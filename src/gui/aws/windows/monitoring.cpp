#include "monitoring.hpp"
#include "gui/imaws.hpp"

#include <imgui.h>
#include <implot.h>

static constexpr ImGuiTreeNodeFlags kDefaultFlags
    = ImGuiTreeNodeFlags_OpenOnArrow
    | ImGuiTreeNodeFlags_OpenOnDoubleClick
    | ImGuiTreeNodeFlags_NavLeftJumpsToParent
    | ImGuiTreeNodeFlags_SpanFullWidth
    | ImGuiTreeNodeFlags_DrawLinesToNodes;

static bool NextTreeNode(const char *label, ImGuiTreeNodeFlags flags) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    return ImGui::TreeNodeEx(label, flags);
}

Aws::CloudWatch::CloudWatchClient ImAws::MonitoringPanel::createCloudWatchClient() {
    auto provider = getSessionCredentialsProvider();

    Aws::Client::ClientConfigurationInitValues clientConfigInitValues;
    clientConfigInitValues.shouldDisableIMDS = true;

    Aws::CloudWatch::CloudWatchClientConfiguration config{clientConfigInitValues};
    config.region = getSessionRegion();

    return Aws::CloudWatch::CloudWatchClient{provider, config};
}

void ImAws::MonitoringPanel::drawMetricNode(MetricSetIterator it) {
    const auto& metric = *it;

    ImGuiTreeNodeFlags flags = kDefaultFlags;

    auto name = metric.GetMetricName();
    if (NextTreeNode(name.c_str(), flags)) {
        for (auto dimension : metric.GetDimensions()) {
            auto id = dimension.GetName();
            auto val = dimension.GetValue();

            if (NextTreeNode(id.c_str(), flags)) {

                if (NextTreeNode(val.c_str(), flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet)) {
                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
}

void ImAws::MonitoringPanel::drawMetricNamespace(MetricMapIterator it) {
    const auto& [ns, metrics] = *it;
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    ImGuiTreeNodeFlags flags = kDefaultFlags;

    if (ImGui::TreeNodeEx(ns.c_str(), flags)) {
        for (auto metric = metrics.begin(); metric != metrics.end(); ++metric) {
            drawMetricNode(metric);
        }
        ImGui::TreePop();
    }
}

void ImAws::MonitoringPanel::draw() {
    bool isFetching = mMetricDescribe.isWorking();
    ImGui::BeginDisabled(isFetching);
    if (ImGui::Button(isFetching ? "Fetching..." : "Fetch Metrics")) {
        mAwsMetrics.clear();
        mUserMetrics.clear();
        mMetricDescribe.run([this](auto&& add, auto&& err, std::stop_token stop) {
            auto cwClient = createCloudWatchClient();

            Aws::CloudWatch::Model::ListMetricsRequest request;
            Aws::String marker;

            do {
                if (!marker.empty()) {
                    request.SetNextToken(marker);
                }

                auto outcome = cwClient.ListMetrics(request);
                if (!outcome.IsSuccess()) {
                    err(outcome.GetError());
                    break;
                }

                const auto& result = outcome.GetResult();
                for (const auto& metric : result.GetMetrics()) {
                    add(metric);
                }

                marker = result.GetNextToken();
            } while (!marker.empty() && !stop.stop_requested());
        });
    }
    ImGui::EndDisabled();

    if (auto metric = mMetricDescribe.pullItem()) {
        if (metric->GetNamespace().starts_with("AWS/")) {
            mAwsMetrics[metric->GetNamespace()].insert(*metric);
        } else {
            mUserMetrics[metric->GetNamespace()].insert(*metric);
        }
    }

    if (mMetricDescribe.hasError()) {
        ImGui::SameLine();
        ImAws::ApiErrorTooltip(mMetricDescribe.error());
    }

    if (ImPlot::BeginPlot("Plot")) {
        ImPlot::SetupAxes("x", "y");
        ImPlot::EndPlot();
    }

    if (ImGui::BeginTable("##MetricTable", 1, ImGuiTableFlags_RowBg)) {
        if (!mUserMetrics.empty()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::SeparatorText("User Metrics");
        }

        for (auto it = mUserMetrics.begin(); it != mUserMetrics.end(); ++it) {
            drawMetricNamespace(it);
        }

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::SeparatorText("AWS Metrics");
        for (auto it = mAwsMetrics.begin(); it != mAwsMetrics.end(); ++it) {
            drawMetricNamespace(it);
        }

        ImGui::EndTable();
    }
}
