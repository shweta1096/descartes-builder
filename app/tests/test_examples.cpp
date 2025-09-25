#include "data/constants.hpp"
#include "data/custom_graph.hpp"
#include "data/tab_manager.hpp"
#include "data/yml_parser.hpp"
#include "ui/main_window.hpp"
#include "ui/models/fdf_block_model.hpp"
#include "ui/models/io_models.hpp"
#include "ui/models/uid_manager.hpp"
#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSignalSpy>

class ExamplesTest : public ::testing::Test
{
protected:
    void SetUp() override { mainWindow = new MainWindow(); }

    void TearDown() override { delete mainWindow; }

    MainWindow *mainWindow = nullptr;
};

double roundTo(double value, int decimalPlaces)
{
    double factor = std::pow(10.0, decimalPlaces);
    return std::round(value * factor) / factor;
}

struct DCBTestCase
{
    QString dcbFile;
    QString expectedScoreFile;
};

inline void PrintTo(const DCBTestCase &param, std::ostream *os)
{
    // added for cleaner test output in case of failure
    *os << param.dcbFile.toStdString();
}

static const std::vector<DCBTestCase> kDcbTests
    = {{"pipe-deformation-train.dcb", "pipe-deformation-train-expected-score.yml"},
       {"pipe-deformation-test.dcb", "pipe-deformation-test-expected-score.yml"},
       {"pipe-deformation-train-compose.dcb", "pipe-deformation-train-expected-score.yml"},
       {"pipe-deformation-test-compose.dcb", "pipe-deformation-test-expected-score.yml"}};

class OpenExampleDCBTest
    : public ExamplesTest
    , public ::testing::WithParamInterface<DCBTestCase>
{};

TEST_P(OpenExampleDCBTest, VerifyScore)
{
    const auto &param = GetParam();
    const QString dcbPath = QDir(QFileInfo(__FILE__).absolutePath() + "/../../examples")
                                .absoluteFilePath(param.dcbFile);
    const QString resultsPath = QDir(QFileInfo(__FILE__).absolutePath() + "/results")
                                    .absoluteFilePath(param.expectedScoreFile);

    QSignalSpy runFinished(mainWindow, SIGNAL(runFinished(bool)));
    QSignalSpy scoreCreated(mainWindow, SIGNAL(scoreParams(QString)));

    ASSERT_TRUE(mainWindow->openDCB(dcbPath))
        << "openDCB failed for " << param.dcbFile.toStdString();

    bool started = mainWindow->executeDCB();
    ASSERT_TRUE(started) << "executeDCB failed to start";

    // wait for engine finished signal to ensure execution completed
    // wait for 2 minutes as sometimes training on GitHub runners is slow
    bool finished = runFinished.wait(2 * constants::MINUTE_MSECS);
    ASSERT_TRUE(finished) << "Timed out waiting for execution to finish";
    bool success = runFinished.takeFirst().at(0).toBool();
    ASSERT_TRUE(success) << "DCB execution finished with failure";

    ASSERT_FALSE(scoreCreated.isEmpty()) << "No scoreParams signal was emitted";
    QString generatedScoreContent = scoreCreated.takeFirst().at(0).toString();
    ASSERT_FALSE(generatedScoreContent.isEmpty()) << "Received score.yml content is empty";

    QFile expectedFile(resultsPath);
    ASSERT_TRUE(expectedFile.open(QIODevice::ReadOnly | QIODevice::Text))
        << "Failed to open expected score.yml: " << param.expectedScoreFile.toStdString();

    const auto expectedMap = parseYml(QString::fromUtf8(expectedFile.readAll()));
    const auto generatedMap = parseYml(generatedScoreContent);

    constexpr double FAULT_TOLERANCE = 1e-2;
    constexpr int DECIMAL_PLACES = 4;
    for (const auto &[key, val] : expectedMap) {
        ASSERT_TRUE(generatedMap.find(key) != generatedMap.end())
            << "Missing key: " << key.toStdString();

        double expectedVal = roundTo(val.toDouble(), DECIMAL_PLACES);
        double generatedVal = roundTo(generatedMap.at(key).toDouble(), DECIMAL_PLACES);

        ASSERT_NEAR(generatedVal, expectedVal, FAULT_TOLERANCE)
            << "Mismatch for " << key.toStdString();
    }
}

INSTANTIATE_TEST_SUITE_P(AllDCBScoreTests,
                         OpenExampleDCBTest,
                         ::testing::ValuesIn(kDcbTests),
                         [](const ::testing::TestParamInfo<DCBTestCase> &info) {
                             QString name = info.param.dcbFile;
                             name = name.replace("-", "_").replace(".", "_"); // needed for gtest
                             return name.toStdString();
                         });
