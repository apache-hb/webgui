#include "monitoring.hpp"
#include "aws/monitoring/model/GetMetricDataRequest.h"
#include "aws/monitoring/model/MetricStat.h"
#include "gui/imaws.hpp"

#include <aws/monitoring/model/MetricDataQuery.h>
#include <aws/monitoring/model/StandardUnit.h>
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

void ImAws::MonitoringPanel::fetchMetricData(const Metric& metric) {
    mMetricDataFetch.run([this, metric = auto{metric}](auto&& add, auto&& err, std::stop_token stop) {
        auto client = createCloudWatchClient();
        auto now = Aws::Utils::DateTime::Now();

        Aws::CloudWatch::Model::MetricStat metricStat;
        metricStat.SetMetric(metric);
        metricStat.SetPeriod(60);
        metricStat.SetStat("Average");
        metricStat.SetUnit(Aws::CloudWatch::Model::StandardUnit::None);

        Aws::CloudWatch::Model::MetricDataQuery query;
        query.SetId("metric");
        query.SetMetricStat(metricStat);

        Aws::CloudWatch::Model::GetMetricDataRequest request;
        request.SetStartTime(now.Millis() - 3600 * 1000); // 1 hour ago
        request.SetEndTime(now);
        request.AddMetricDataQueries(query);
        request.SetMaxDatapoints(500);

        Aws::String nextToken;
        do {
            if (!nextToken.empty()) {
                request.SetNextToken(nextToken);
            }

            auto outcome = client.GetMetricData(request);
            if (!outcome.IsSuccess()) {
                err(outcome.GetError());
                break;
            }

            const auto& result = outcome.GetResult();
            add(result);

            nextToken = result.GetNextToken();
        } while (!nextToken.empty() && !stop.stop_requested());
    });
}

void ImAws::MonitoringPanel::drawMetricNode(MetricSetIterator it) {
    const auto& metric = *it;

    auto name = metric.GetMetricName();
    if (NextTreeNode(name.c_str(), kDefaultFlags)) {
        for (auto dimension : metric.GetDimensions()) {
            auto id = dimension.GetName();
            auto val = dimension.GetValue();

            ImGui::AlignTextToFramePadding();
            if (NextTreeNode(id.c_str(), kDefaultFlags | ImGuiTreeNodeFlags_AllowOverlap)) {

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGui::TextUnformatted(val.c_str());
                ImGui::SameLine();
                if (ImGui::Button("Graph")) {
                    mMetricName = name;
                    fetchMetricData(metric);
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

    if (ImGui::TreeNodeEx(ns.c_str(), kDefaultFlags)) {
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

    ImGui::SameLine();
    ImGui::Text("Data Points: %zu", mPlotXData.size());

    if (mMetricDescribe.hasError()) {
        ImGui::SameLine();
        ImAws::ApiErrorTooltip(mMetricDescribe.error());
    } else if (mMetricDataFetch.hasError()) {
        ImGui::SameLine();
        ImAws::ApiErrorTooltip(mMetricDataFetch.error());
    }

    if (auto next = mMetricDataFetch.pullItem()) {
        for (const auto& result : next->GetMetricDataResults()) {
            if (result.GetId() == mMetricName) {
                for (const auto& timestamp : result.GetTimestamps()) {
                    mPlotXData.push_back(static_cast<float>(timestamp.Millis() / 1000.0));
                }

                for (const auto& value : result.GetValues()) {
                    mPlotYData.push_back(static_cast<float>(value));
                }
            }
        }
    }

    if (ImPlot::BeginPlot("Plot")) {
        ImPlotAxisFlags flags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;
        ImPlot::SetupAxes("Time", mMetricName.c_str(), flags, flags);

        if (!mPlotXData.empty() && !mPlotYData.empty()) {
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
            assert(mPlotXData.size() == mPlotYData.size());
            ImPlot::PlotLine(mMetricName.c_str(), mPlotXData.data(), mPlotYData.data(), static_cast<int>(mPlotXData.size()));
        }

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
