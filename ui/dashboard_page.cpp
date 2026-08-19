#include "dashboard_page.h"
#include "app_config.h"

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QVBoxLayout>
#include <algorithm>

class TimeSeriesWidget : public QWidget {
public:
    TimeSeriesWidget(const QString &title, const QString &unit,
                     const QColor &color, bool digital = false,
                     QWidget *parent = nullptr)
        : QWidget(parent), title_(title), unit_(unit), color_(color), digital_(digital)
    {
        setMinimumHeight(135);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void add(double value)
    {
        points_.append(value);
        while (points_.size() > AppConfig::MaxChartPoints)
            points_.removeFirst();
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QPainterPath card;
        card.addRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5),
                            12, 12);
        painter.fillPath(card, QColor("#ffffff"));
        painter.setPen(QPen(QColor("#e2e8f0"), 1));
        painter.drawPath(card);

        painter.setPen(QColor("#0f172a"));
        QFont titleFont = painter.font();
        titleFont.setBold(true);
        titleFont.setPointSize(10);
        painter.setFont(titleFont);
        painter.drawText(16, 24, title_);

        if (!points_.isEmpty()) {
            painter.setPen(color_);
            QFont valueFont = painter.font();
            valueFont.setPointSize(10);
            painter.setFont(valueFont);
            const QString current = digital_
                ? (points_.last() > 0.5 ? QStringLiteral("PHÁT HIỆN")
                                        : QStringLiteral("AN TOÀN"))
                : QStringLiteral("%1 %2")
                      .arg(points_.last(), 0, 'f', unit_ == QStringLiteral("ADC") ? 0 : 1)
                      .arg(unit_);
            painter.drawText(QRect(0, 10, width() - 16, 20), Qt::AlignRight,
                             current);
        }

        const QRectF area(16, 38, width() - 32, height() - 54);
        painter.setPen(QPen(QColor("#eef2f7"), 1));
        for (int line = 0; line <= 3; ++line) {
            const qreal y = area.top() + area.height() * line / 3.0;
            painter.drawLine(QPointF(area.left(), y), QPointF(area.right(), y));
        }

        if (points_.size() < 2) {
            painter.setPen(QColor("#94a3b8"));
            painter.drawText(area, Qt::AlignCenter,
                             QStringLiteral("Đang chờ thêm dữ liệu…"));
            return;
        }

        const auto [minIt, maxIt] = std::minmax_element(points_.cbegin(),
                                                        points_.cend());
        double minimum = digital_ ? -0.1 : *minIt;
        double maximum = digital_ ? 1.1 : *maxIt;
        if (!digital_ && qFuzzyCompare(minimum, maximum)) {
            minimum -= 1.0;
            maximum += 1.0;
        } else {
            const double padding = (maximum - minimum) * 0.12;
            minimum -= padding;
            maximum += padding;
        }

        QPainterPath linePath;
        for (int i = 0; i < points_.size(); ++i) {
            const double x = area.left() + area.width() * i / (points_.size() - 1);
            const double y = area.bottom()
                - area.height() * (points_[i] - minimum) / (maximum - minimum);
            i == 0 ? linePath.moveTo(x, y) : linePath.lineTo(x, y);
        }

        QPainterPath fillPath = linePath;
        fillPath.lineTo(area.right(), area.bottom());
        fillPath.lineTo(area.left(), area.bottom());
        fillPath.closeSubpath();
        QLinearGradient fill(0, area.top(), 0, area.bottom());
        QColor translucent = color_;
        translucent.setAlpha(55);
        fill.setColorAt(0, translucent);
        translucent.setAlpha(4);
        fill.setColorAt(1, translucent);
        painter.fillPath(fillPath, fill);

        painter.setPen(QPen(color_, 2.2, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));
        painter.drawPath(linePath);
        painter.setPen(Qt::NoPen);
        painter.setBrush(color_);
        painter.drawEllipse(linePath.currentPosition(), 3.5, 3.5);
    }

private:
    QString title_;
    QString unit_;
    QColor color_;
    bool digital_ = false;
    QList<double> points_;
};

static QFrame *metricCard(const QString &title, const QString &initialValue,
                          const QString &caption, const QColor &accent,
                          QLabel **valueLabel)
{
    auto *frame = new QFrame;
    frame->setObjectName(QStringLiteral("metricCard"));
    frame->setMinimumHeight(116);

    auto *stripe = new QFrame;
    stripe->setFixedWidth(4);
    stripe->setStyleSheet(QStringLiteral("background:%1;border-radius:2px;")
                              .arg(accent.name()));
    auto *titleLabel = new QLabel(title);
    titleLabel->setObjectName(QStringLiteral("metricTitle"));
    auto *value = new QLabel(initialValue);
    value->setObjectName(QStringLiteral("metricValue"));
    auto *captionLabel = new QLabel(caption);
    captionLabel->setObjectName(QStringLiteral("metricCaption"));

    auto *text = new QVBoxLayout;
    text->setContentsMargins(12, 10, 12, 10);
    text->setSpacing(3);
    text->addWidget(titleLabel);
    text->addWidget(value, 1);
    text->addWidget(captionLabel);

    auto *layout = new QHBoxLayout(frame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(stripe);
    layout->addLayout(text, 1);
    *valueLabel = value;
    return frame;
}

DashboardPage::DashboardPage(QWidget *parent) : QWidget(parent)
{
    auto *heading = new QLabel(QStringLiteral("Chất lượng không khí"));
    heading->setObjectName(QStringLiteral("pageHeading"));
    auto *subheading = new QLabel(
        QStringLiteral("Theo dõi dữ liệu môi trường theo thời gian thực"));
    subheading->setObjectName(QStringLiteral("pageSubheading"));

    auto *titleLayout = new QVBoxLayout;
    titleLayout->setSpacing(2);
    titleLayout->addWidget(heading);
    titleLayout->addWidget(subheading);

    auto *cards = new QGridLayout;
    cards->setSpacing(12);
    cards->addWidget(metricCard(QStringLiteral("NHIỆT ĐỘ"), QStringLiteral("-- °C"),
                                QStringLiteral("Cảm biến SHT3x"), QColor("#ef4444"),
                                &temperature_), 0, 0);
    cards->addWidget(metricCard(QStringLiteral("ĐỘ ẨM"), QStringLiteral("-- %"),
                                QStringLiteral("Độ ẩm tương đối"), QColor("#3b82f6"),
                                &humidity_), 0, 1);
    cards->addWidget(metricCard(QStringLiteral("NỒNG ĐỘ KHÍ MQ-2 (AO)"), QStringLiteral("-- ADC"),
                                QStringLiteral("Trung bình 30 mẫu • ngưỡng 2500 ADC"), QColor("#8b5cf6"),
                                &mq2_), 0, 2);
    cards->setColumnStretch(0, 1);
    cards->setColumnStretch(1, 1);
    cards->setColumnStretch(2, 1);

    quality_ = new QLabel(QStringLiteral("ĐANG CHỜ DỮ LIỆU"));
    quality_->setAlignment(Qt::AlignCenter);
    quality_->setMinimumHeight(42);
    quality_->setObjectName(QStringLiteral("qualityWaiting"));

    esp32_ = new QLabel(QStringLiteral("●  ESP32: CHƯA XÁC ĐỊNH"));
    mqtt_ = new QLabel(QStringLiteral("●  MQTT: MẤT KẾT NỐI"));
    lastSeen_ = new QLabel(QStringLiteral("Cập nhật cuối: --"));
    esp32_->setObjectName(QStringLiteral("statusOffline"));
    mqtt_->setObjectName(QStringLiteral("statusOffline"));
    lastSeen_->setObjectName(QStringLiteral("lastSeen"));

    auto *status = new QHBoxLayout;
    status->setSpacing(16);
    status->addWidget(esp32_);
    status->addWidget(mqtt_);
    status->addStretch();
    status->addWidget(lastSeen_);

    tempChart_ = new TimeSeriesWidget(QStringLiteral("Nhiệt độ theo thời gian"),
                                      QStringLiteral("°C"), QColor("#ef4444"));
    humidityChart_ = new TimeSeriesWidget(QStringLiteral("Độ ẩm theo thời gian"),
                                          QStringLiteral("%"), QColor("#3b82f6"));
    mq2Chart_ = new TimeSeriesWidget(QStringLiteral("MQ-2 trung bình theo thời gian"),
                                     QStringLiteral("ADC"), QColor("#8b5cf6"));

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 18, 20, 20);
    layout->setSpacing(12);
    layout->addLayout(titleLayout);
    layout->addLayout(cards);
    layout->addWidget(quality_);
    layout->addLayout(status);
    layout->addWidget(tempChart_, 1);
    layout->addWidget(humidityChart_, 1);
    layout->addWidget(mq2Chart_, 1);
}

void DashboardPage::updateReading(const SensorReading &reading)
{
    temperature_->setText(QStringLiteral("%1 °C").arg(reading.temperature, 0, 'f', 1));
    humidity_->setText(QStringLiteral("%1 %").arg(reading.humidity, 0, 'f', 1));
    mq2_->setText(!reading.mq2DigitalMode
        ? QStringLiteral("%1 ADC  ·  %2 mV").arg(reading.mq2Raw).arg(reading.mq2Mv)
        : QStringLiteral("CHƯA CÓ DỮ LIỆU AO"));
    quality_->setText(reading.alarm ? QStringLiteral("⚠  CẢNH BÁO KHÍ / KHÓI")
                                    : QStringLiteral("✓  CHẤT LƯỢNG KHÔNG KHÍ BÌNH THƯỜNG"));
    quality_->setObjectName(reading.alarm ? QStringLiteral("qualityAlarm")
                                         : QStringLiteral("qualityNormal"));
    quality_->style()->unpolish(quality_);
    quality_->style()->polish(quality_);
    lastSeen_->setText(QStringLiteral("Cập nhật cuối: %1")
                           .arg(reading.timestamp.toString("dd/MM/yyyy  HH:mm:ss")));
    tempChart_->add(reading.temperature);
    humidityChart_->add(reading.humidity);
    mq2Chart_->add(!reading.mq2DigitalMode ? reading.mq2Raw : 0);
}

void DashboardPage::setMq2Pending(bool pending)
{
    if (!pending)
        return;
    quality_->setText(QStringLiteral("…  ĐANG XÁC NHẬN NGƯỠNG ADC TỪ MQ-2"));
    quality_->setObjectName(QStringLiteral("qualityPending"));
    quality_->style()->unpolish(quality_);
    quality_->style()->polish(quality_);
}

void DashboardPage::setMq2Ready(bool ready)
{
    if (ready)
        return;
    quality_->setText(QStringLiteral("…  MQ-2 ĐANG LÀM NÓNG VÀ ĐỌC GIÁ TRỊ AO"));
    quality_->setObjectName(QStringLiteral("qualityPending"));
    quality_->style()->unpolish(quality_);
    quality_->style()->polish(quality_);
}

void DashboardPage::setMq2FirmwareCompatible(bool compatible)
{
    if (compatible)
        return;
    quality_->setText(QStringLiteral("⚠  ESP32 CHƯA CHẠY FIRMWARE MQ-2 CHẾ ĐỘ AO"));
    quality_->setObjectName(QStringLiteral("qualityPending"));
    quality_->style()->unpolish(quality_);
    quality_->style()->polish(quality_);
}

void DashboardPage::setMqttConnected(bool connected)
{
    mqtt_->setText(connected ? QStringLiteral("●  MQTT: ĐÃ KẾT NỐI")
                             : QStringLiteral("●  MQTT: MẤT KẾT NỐI"));
    mqtt_->setObjectName(connected ? QStringLiteral("statusOnline")
                                   : QStringLiteral("statusOffline"));
    mqtt_->style()->unpolish(mqtt_);
    mqtt_->style()->polish(mqtt_);
}

void DashboardPage::setDeviceStatus(const QString &status)
{
    const bool online = status == QStringLiteral("ONLINE");
    esp32_->setText(online ? QStringLiteral("●  ESP32: TRỰC TUYẾN")
                           : QStringLiteral("●  ESP32: %1").arg(status));
    esp32_->setObjectName(online ? QStringLiteral("statusOnline")
                                 : QStringLiteral("statusOffline"));
    esp32_->style()->unpolish(esp32_);
    esp32_->style()->polish(esp32_);
}

void DashboardPage::setDataStale(bool stale)
{
    if (!stale)
        return;
    esp32_->setText(QStringLiteral("●  ESP32: DỮ LIỆU QUÁ HẠN"));
    esp32_->setObjectName(QStringLiteral("statusWarning"));
    esp32_->style()->unpolish(esp32_);
    esp32_->style()->polish(esp32_);
}
