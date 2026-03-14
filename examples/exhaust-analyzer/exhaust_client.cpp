// exhaust_client.cpp
//
// Qt5 GUI for the exhaust gas analyzer. Displays live sensor readings
// pushed via IPC notifications, with color-coded gauges that resemble
// a real emissions bench display.

#include "ExhaustAnalyzer.h"

#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMetaType>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <cstdio>

using namespace ms::ipc;

Q_DECLARE_METATYPE(GasReadings)
Q_DECLARE_METATYPE(AnalyzerStatus)

// ── Gauge widget ────────────────────────────────────────────────────
// A single sensor display: name, value, and unit, with color coding.

class GaugeWidget : public QGroupBox
{
    Q_OBJECT

public:
    GaugeWidget(const QString &name, const QString &unit, QWidget *parent = nullptr)
        : QGroupBox(parent), m_unit(unit)
    {
        auto *layout = new QVBoxLayout(this);
        layout->setSpacing(2);

        m_nameLabel = new QLabel(name, this);
        m_nameLabel->setAlignment(Qt::AlignCenter);
        QFont nameFont = m_nameLabel->font();
        nameFont.setPointSize(10);
        nameFont.setBold(true);
        m_nameLabel->setFont(nameFont);

        m_valueLabel = new QLabel("---", this);
        m_valueLabel->setAlignment(Qt::AlignCenter);
        QFont valFont("Monospace", 22, QFont::Bold);
        valFont.setStyleHint(QFont::Monospace);
        m_valueLabel->setFont(valFont);
        m_valueLabel->setMinimumWidth(140);

        m_unitLabel = new QLabel(unit, this);
        m_unitLabel->setAlignment(Qt::AlignCenter);
        QFont unitFont = m_unitLabel->font();
        unitFont.setPointSize(9);
        m_unitLabel->setFont(unitFont);
        m_unitLabel->setStyleSheet("color: #888;");

        layout->addWidget(m_nameLabel);
        layout->addWidget(m_valueLabel);
        layout->addWidget(m_unitLabel);

        setStyleSheet(
            "GaugeWidget { border: 2px solid #444; border-radius: 8px; "
            "padding: 8px; margin: 4px; background: #1a1a1a; }"
        );
    }

    void setValue(double value, const QString &format, double warnLow, double warnHigh,
                  double critLow, double critHigh)
    {
        m_valueLabel->setText(format.arg(value));

        QString color = "#00cc66"; // green
        if (value < critLow || value > critHigh)
            color = "#ff3333"; // red
        else if (value < warnLow || value > warnHigh)
            color = "#ffaa00"; // amber

        m_valueLabel->setStyleSheet(QString("color: %1;").arg(color));
    }

    void setInactive()
    {
        m_valueLabel->setText("---");
        m_valueLabel->setStyleSheet("color: #666;");
    }

private:
    QLabel *m_nameLabel;
    QLabel *m_valueLabel;
    QLabel *m_unitLabel;
    QString m_unit;
};

// ── IPC client with Qt signals ──────────────────────────────────────
// The generated onReadingsUpdated/onStatusChanged callbacks fire on
// the IPC receiver thread. We emit Qt signals which cross to the GUI
// thread via QueuedConnection.

class AnalyzerClient : public QObject, public ExhaustAnalyzer
{
    Q_OBJECT

public:
    AnalyzerClient(const char *name, QObject *parent = nullptr)
        : QObject(parent), ExhaustAnalyzer(name)
    {
    }

signals:
    void readingsReceived(GasReadings readings);
    void statusReceived(AnalyzerStatus status);

protected:
    void onReadingsUpdated(GasReadings readings) override
    {
        emit readingsReceived(readings);
    }

    void onStatusChanged(AnalyzerStatus status) override
    {
        emit statusReceived(status);
    }
};

// ── Main window ─────────────────────────────────────────────────────

class AnalyzerWindow : public QWidget
{
    Q_OBJECT

public:
    AnalyzerWindow(QWidget *parent = nullptr) : QWidget(parent)
    {
        setWindowTitle("Exhaust Gas Analyzer — ms-ipc");
        setStyleSheet("background: #111; color: #ddd;");
        setMinimumSize(680, 520);

        auto *root = new QVBoxLayout(this);

        // ── Title bar ───────────────────────────────────────────
        auto *titleBar = new QHBoxLayout();
        auto *title = new QLabel("EXHAUST GAS ANALYZER");
        QFont titleFont("Monospace", 16, QFont::Bold);
        titleFont.setStyleHint(QFont::Monospace);
        title->setFont(titleFont);
        title->setStyleSheet("color: #0af;");

        m_statusLabel = new QLabel("OFFLINE");
        m_statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QFont statusFont("Monospace", 12, QFont::Bold);
        statusFont.setStyleHint(QFont::Monospace);
        m_statusLabel->setFont(statusFont);
        m_statusLabel->setStyleSheet("color: #888;");

        titleBar->addWidget(title);
        titleBar->addStretch();
        titleBar->addWidget(m_statusLabel);
        root->addLayout(titleBar);

        // ── Gauge grid ──────────────────────────────────────────
        auto *grid = new QGridLayout();
        grid->setSpacing(6);

        m_o2 = new GaugeWidget("O\u2082", "%");
        m_co = new GaugeWidget("CO", "%");
        m_co2 = new GaugeWidget("CO\u2082", "%");
        m_nox = new GaugeWidget("NOx", "ppm");
        m_hc = new GaugeWidget("HC", "ppm");
        m_lambda = new GaugeWidget("\u03BB", "");
        m_temp = new GaugeWidget("EGT", "\u00B0C");

        grid->addWidget(m_o2, 0, 0);
        grid->addWidget(m_co, 0, 1);
        grid->addWidget(m_co2, 0, 2);
        grid->addWidget(m_nox, 1, 0);
        grid->addWidget(m_hc, 1, 1);
        grid->addWidget(m_lambda, 1, 2);
        grid->addWidget(m_temp, 2, 0);

        root->addLayout(grid);

        // ── Controls ────────────────────────────────────────────
        auto *controls = new QHBoxLayout();
        controls->addStretch();

        m_startBtn = new QPushButton("START");
        m_startBtn->setFixedSize(110, 36);
        m_startBtn->setStyleSheet(
            "QPushButton { background: #004400; color: #0c0; border: 1px solid #0a0; "
            "border-radius: 4px; font-weight: bold; font-size: 13px; }"
            "QPushButton:hover { background: #006600; }"
            "QPushButton:disabled { background: #222; color: #555; border-color: #333; }");

        m_stopBtn = new QPushButton("STOP");
        m_stopBtn->setFixedSize(110, 36);
        m_stopBtn->setStyleSheet(
            "QPushButton { background: #440000; color: #c00; border: 1px solid #a00; "
            "border-radius: 4px; font-weight: bold; font-size: 13px; }"
            "QPushButton:hover { background: #660000; }"
            "QPushButton:disabled { background: #222; color: #555; border-color: #333; }");

        m_connectBtn = new QPushButton("CONNECT");
        m_connectBtn->setFixedSize(130, 36);
        m_connectBtn->setStyleSheet(
            "QPushButton { background: #002244; color: #0af; border: 1px solid #08f; "
            "border-radius: 4px; font-weight: bold; font-size: 13px; }"
            "QPushButton:hover { background: #003366; }"
            "QPushButton:disabled { background: #222; color: #555; border-color: #333; }");

        controls->addWidget(m_startBtn);
        controls->addWidget(m_stopBtn);
        controls->addWidget(m_connectBtn);
        controls->addStretch();
        root->addLayout(controls);

        // ── Info bar ────────────────────────────────────────────
        m_infoLabel = new QLabel("Not connected");
        m_infoLabel->setStyleSheet("color: #666; font-size: 10px; padding: 4px;");
        root->addWidget(m_infoLabel);

        // ── Client setup ────────────────────────────────────────
        m_client = new AnalyzerClient("exhaust_analyzer", this);

        QObject::connect(m_client, &AnalyzerClient::readingsReceived,
                         this, &AnalyzerWindow::onReadings, Qt::QueuedConnection);
        QObject::connect(m_client, &AnalyzerClient::statusReceived,
                         this, &AnalyzerWindow::onStatus, Qt::QueuedConnection);

        QObject::connect(m_connectBtn, &QPushButton::clicked, this, &AnalyzerWindow::onConnectClicked);
        QObject::connect(m_startBtn, &QPushButton::clicked, this, &AnalyzerWindow::onStartClicked);
        QObject::connect(m_stopBtn, &QPushButton::clicked, this, &AnalyzerWindow::onStopClicked);

        m_startBtn->setEnabled(false);
        m_stopBtn->setEnabled(false);

        // Heartbeat timer — poll status every second as a reconnect check.
        m_heartbeat = new QTimer(this);
        QObject::connect(m_heartbeat, &QTimer::timeout, this, &AnalyzerWindow::onHeartbeat);
    }

    ~AnalyzerWindow() override
    {
        m_heartbeat->stop();
        m_client->ExhaustAnalyzer::disconnect();
    }

private slots:
    void onReadings(GasReadings r)
    {
        m_o2->setValue(r.o2Percent, "%1", 0.3, 3.0, 0.0, 5.0);
        m_co->setValue(r.coPercent, "%1", 0.0, 3.0, -1.0, 5.0);
        m_co2->setValue(r.co2Percent, "%1", 12.0, 15.0, 10.0, 16.5);
        m_nox->setValue(r.noxPpm, "%1", 0.0, 2000.0, -1.0, 3000.0);
        m_hc->setValue(r.hcPpm, "%1", 0.0, 300.0, -1.0, 500.0);
        m_lambda->setValue(r.lambda, "%1", 0.95, 1.05, 0.90, 1.10);
        m_temp->setValue(r.exhaustTempC, "%1", 200.0, 600.0, 100.0, 800.0);

        m_sampleCount++;
        m_infoLabel->setText(QString("Samples received: %1").arg(m_sampleCount));
    }

    void onStatus(AnalyzerStatus status)
    {
        updateStatusDisplay(status);
    }

    void onConnectClicked()
    {
        if (m_client->isConnected())
        {
            m_client->ExhaustAnalyzer::disconnect();
            m_heartbeat->stop();
            m_connectBtn->setText("CONNECT");
            m_statusLabel->setText("OFFLINE");
            m_statusLabel->setStyleSheet("color: #888;");
            m_startBtn->setEnabled(false);
            m_stopBtn->setEnabled(false);
            m_infoLabel->setText("Disconnected");
            setAllGaugesInactive();
        }
        else
        {
            if (m_client->ExhaustAnalyzer::connect())
            {
                m_connectBtn->setText("DISCONNECT");
                m_infoLabel->setText("Connected — polling status...");
                m_heartbeat->start(1000);
                onHeartbeat();
            }
            else
            {
                m_infoLabel->setText("Connection failed — is the server running?");
            }
        }
    }

    void onStartClicked()
    {
        bool started = false;
        int rc = m_client->StartMeasurement(&started);
        if (rc == IPC_SUCCESS && started)
        {
            m_infoLabel->setText("Measurement started");
        }
        else
        {
            m_infoLabel->setText("Could not start measurement");
        }
    }

    void onStopClicked()
    {
        bool stopped = false;
        int rc = m_client->StopMeasurement(&stopped);
        if (rc == IPC_SUCCESS && stopped)
        {
            m_infoLabel->setText("Measurement stopped");
            setAllGaugesInactive();
        }
    }

    void onHeartbeat()
    {
        if (!m_client->isConnected())
        {
            m_heartbeat->stop();
            m_connectBtn->setText("CONNECT");
            m_statusLabel->setText("OFFLINE");
            m_statusLabel->setStyleSheet("color: #888;");
            m_startBtn->setEnabled(false);
            m_stopBtn->setEnabled(false);
            m_infoLabel->setText("Connection lost");
            setAllGaugesInactive();
            return;
        }

        AnalyzerStatus status{};
        int rc = m_client->GetStatus(&status);
        if (rc == IPC_SUCCESS)
        {
            updateStatusDisplay(status);
            m_heartbeatFailures = 0;
        }
        else
        {
            m_heartbeatFailures++;
            m_infoLabel->setText(QString("Status poll failed (%1)").arg(rc));
            if (m_heartbeatFailures >= 3)
            {
                m_heartbeat->stop();
                m_client->ExhaustAnalyzer::disconnect();
                m_connectBtn->setText("CONNECT");
                m_statusLabel->setText("OFFLINE");
                m_statusLabel->setStyleSheet("color: #888;");
                m_startBtn->setEnabled(false);
                m_stopBtn->setEnabled(false);
                m_infoLabel->setText("Server unresponsive — disconnected");
                setAllGaugesInactive();
            }
        }
    }

private:
    void updateStatusDisplay(AnalyzerStatus status)
    {
        struct StatusInfo
        {
            const char *text;
            const char *color;
            bool startEnabled;
            bool stopEnabled;
        };

        static const StatusInfo info[] = {
            {"OFFLINE", "#888", false, false},
            {"WARMING", "#fa0", false, false},
            {"READY", "#0c0", true, false},
            {"MEASURING", "#0af", false, true},
            {"ERROR", "#f00", false, false},
        };

        auto idx = static_cast<unsigned>(status);
        if (idx >= 5) idx = 4;

        m_statusLabel->setText(info[idx].text);
        m_statusLabel->setStyleSheet(
            QString("color: %1; font-weight: bold;").arg(info[idx].color));
        m_startBtn->setEnabled(info[idx].startEnabled);
        m_stopBtn->setEnabled(info[idx].stopEnabled);
    }

    void setAllGaugesInactive()
    {
        m_o2->setInactive();
        m_co->setInactive();
        m_co2->setInactive();
        m_nox->setInactive();
        m_hc->setInactive();
        m_lambda->setInactive();
        m_temp->setInactive();
    }

    AnalyzerClient *m_client;
    QTimer *m_heartbeat;

    QLabel *m_statusLabel;
    QLabel *m_infoLabel;
    QPushButton *m_startBtn;
    QPushButton *m_stopBtn;
    QPushButton *m_connectBtn;

    GaugeWidget *m_o2;
    GaugeWidget *m_co;
    GaugeWidget *m_co2;
    GaugeWidget *m_nox;
    GaugeWidget *m_hc;
    GaugeWidget *m_lambda;
    GaugeWidget *m_temp;

    int m_sampleCount = 0;
    int m_heartbeatFailures = 0;
};

// ── main ────────────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    qRegisterMetaType<GasReadings>("GasReadings");
    qRegisterMetaType<AnalyzerStatus>("AnalyzerStatus");

    QApplication app(argc, argv);
    app.setApplicationName("Exhaust Gas Analyzer");

    AnalyzerWindow window;
    window.show();

    return app.exec();
}

#include "exhaust_client.moc"
