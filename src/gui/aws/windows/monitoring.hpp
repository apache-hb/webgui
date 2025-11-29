#pragma once

#include "gui/aws/window.hpp"
#include "util/stream.hpp"

#include <aws/monitoring/CloudWatchClient.h>

namespace ImAws {
    class MonitoringPanel final : public IWindow {
        using Metric = Aws::CloudWatch::Model::Metric;
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

        sm::AsyncStream<Metric, CloudWatchError> mMetricDescribe;
        MetricMap mAwsMetrics;
        MetricMap mUserMetrics;
        MetricMapIterator mSelectedNode;

        Aws::CloudWatch::CloudWatchClient createCloudWatchClient();

        void drawMetricNode(MetricSetIterator it);
        void drawMetricNamespace(MetricMapIterator it);
    public:
        using IWindow::IWindow;

        void draw() override;
    };
}
