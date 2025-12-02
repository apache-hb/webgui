#pragma once

#include "gui/aws/errors.hpp"
#include "gui/aws/window.hpp"
#include "util/stream.hpp"

#include <aws/monitoring/CloudWatchClient.h>

namespace ImAws {
    struct MetricPlot {
        std::string metricNamespace;
        std::string metricName;
        std::string displayName;
        std::vector<float> xData;
        std::vector<float> yData;
        bool enabled;
    };

    class MonitoringPanel final : public IWindow {
        using Metric = Aws::CloudWatch::Model::Metric;
        using GetMetricDataResult = Aws::CloudWatch::Model::GetMetricDataResult;
        using CloudWatchError = Aws::CloudWatch::CloudWatchError;

        struct MetricNameCompare {
            bool operator()(const Metric& a, const Metric& b) const {
                return a.GetMetricName() < b.GetMetricName();
            }
        };

        using MetricSet = std::set<Metric, MetricNameCompare>;
        using MetricMap = std::map<std::string, MetricSet>;
        using MetricSetIterator = MetricSet::iterator;
        using MetricMapIterator = MetricMap::iterator;

        sm::ErrorPanel mErrorPanel;
        sm::AsyncStream<Metric, CloudWatchError> mMetricDescribe;
        MetricMap mAwsMetrics;
        MetricMap mUserMetrics;
        MetricMapIterator mSelectedNode;

        sm::AsyncStream<GetMetricDataResult, CloudWatchError> mMetricDataFetch;

        bool mAutoFit = false;
        std::string mMetricName = "Metric";
        std::vector<float> mPlotXData;
        std::vector<float> mPlotYData;

        Aws::CloudWatch::CloudWatchClient createCloudWatchClient();

        void fetchMetricData(const Metric& metric);

        void drawMetricNode(MetricSetIterator it);
        void drawMetricNamespace(MetricMapIterator it);
    public:
        using IWindow::IWindow;

        void draw() override;
    };
}
