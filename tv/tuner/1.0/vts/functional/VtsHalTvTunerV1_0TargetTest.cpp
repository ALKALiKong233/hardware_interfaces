/*
 * Copyright 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "VtsHalTvTunerV1_0TargetTest.h"

namespace {

AssertionResult TunerBroadcastHidlTest::filterDataOutputTest(vector<string> /*goldenOutputFiles*/) {
    return filterDataOutputTestBase(mFilterTests);
}

AssertionResult TunerPlaybackHidlTest::filterDataOutputTest(vector<string> /*goldenOutputFiles*/) {
    return filterDataOutputTestBase(mFilterTests);
}

AssertionResult TunerDescramblerHidlTest::filterDataOutputTest(
        vector<string> /*goldenOutputFiles*/) {
    return filterDataOutputTestBase(mFilterTests);
}

void TunerFilterHidlTest::configSingleFilterInDemuxTest(FilterConfig filterConf,
                                                        FrontendConfig frontendConf) {
    uint32_t feId;
    uint32_t demuxId;
    sp<IDemux> demux;
    uint32_t filterId;

    mFrontendTests.getFrontendIdByType(frontendConf.type, feId);
    ASSERT_TRUE(feId != INVALID_ID);
    ASSERT_TRUE(mFrontendTests.openFrontendById(feId));
    ASSERT_TRUE(mFrontendTests.setFrontendCallback());
    ASSERT_TRUE(mDemuxTests.openDemux(demux, demuxId));
    ASSERT_TRUE(mDemuxTests.setDemuxFrontendDataSource(feId));
    mFilterTests.setDemux(demux);
    ASSERT_TRUE(mFilterTests.openFilterInDemux(filterConf.type, filterConf.bufferSize));
    ASSERT_TRUE(mFilterTests.getNewlyOpenedFilterId(filterId));
    ASSERT_TRUE(mFilterTests.configFilter(filterConf.settings, filterId));
    ASSERT_TRUE(mFilterTests.getFilterMQDescriptor(filterId));
    ASSERT_TRUE(mFilterTests.startFilter(filterId));
    ASSERT_TRUE(mFilterTests.stopFilter(filterId));
    ASSERT_TRUE(mFilterTests.closeFilter(filterId));
    ASSERT_TRUE(mDemuxTests.closeDemux());
    ASSERT_TRUE(mFrontendTests.closeFrontend());
}

void TunerBroadcastHidlTest::broadcastSingleFilterTest(FilterConfig filterConf,
                                                       FrontendConfig frontendConf) {
    uint32_t feId;
    uint32_t demuxId;
    sp<IDemux> demux;
    uint32_t filterId;

    mFrontendTests.getFrontendIdByType(frontendConf.type, feId);
    if (feId == INVALID_ID) {
        // TODO broadcast test on Cuttlefish needs licensed ts input,
        // these tests are runnable on vendor device with real frontend module
        // or with manual ts installing and use DVBT frontend.
        return;
    }
    ASSERT_TRUE(mFrontendTests.openFrontendById(feId));
    ASSERT_TRUE(mFrontendTests.setFrontendCallback());
    ASSERT_TRUE(mDemuxTests.openDemux(demux, demuxId));
    ASSERT_TRUE(mDemuxTests.setDemuxFrontendDataSource(feId));
    mFilterTests.setDemux(demux);
    ASSERT_TRUE(mFilterTests.openFilterInDemux(filterConf.type, filterConf.bufferSize));
    ASSERT_TRUE(mFilterTests.getNewlyOpenedFilterId(filterId));
    ASSERT_TRUE(mFilterTests.configFilter(filterConf.settings, filterId));
    ASSERT_TRUE(mFilterTests.getFilterMQDescriptor(filterId));
    ASSERT_TRUE(mFilterTests.startFilter(filterId));
    // tune test
    ASSERT_TRUE(mFrontendTests.tuneFrontend(frontendConf));
    ASSERT_TRUE(filterDataOutputTest(goldenOutputFiles));
    ASSERT_TRUE(mFrontendTests.stopTuneFrontend());
    ASSERT_TRUE(mFilterTests.stopFilter(filterId));
    ASSERT_TRUE(mFilterTests.closeFilter(filterId));
    ASSERT_TRUE(mDemuxTests.closeDemux());
    ASSERT_TRUE(mFrontendTests.closeFrontend());
}

void TunerPlaybackHidlTest::playbackSingleFilterTest(FilterConfig filterConf, DvrConfig dvrConf) {
    uint32_t demuxId;
    sp<IDemux> demux;
    uint32_t filterId;

    ASSERT_TRUE(mDemuxTests.openDemux(demux, demuxId));
    mFilterTests.setDemux(demux);
    mDvrTests.setDemux(demux);
    ASSERT_TRUE(mDvrTests.openDvrInDemux(dvrConf.type, dvrConf.bufferSize));
    ASSERT_TRUE(mDvrTests.configDvr(dvrConf.settings));
    ASSERT_TRUE(mDvrTests.getDvrMQDescriptor());
    ASSERT_TRUE(mFilterTests.openFilterInDemux(filterConf.type, filterConf.bufferSize));
    ASSERT_TRUE(mFilterTests.getNewlyOpenedFilterId(filterId));
    ASSERT_TRUE(mFilterTests.configFilter(filterConf.settings, filterId));
    ASSERT_TRUE(mFilterTests.getFilterMQDescriptor(filterId));
    mDvrTests.startPlaybackInputThread(dvrConf.playbackInputFile, dvrConf.settings.playback());
    ASSERT_TRUE(mDvrTests.startDvr());
    ASSERT_TRUE(mFilterTests.startFilter(filterId));
    ASSERT_TRUE(filterDataOutputTest(goldenOutputFiles));
    mDvrTests.stopPlaybackThread();
    ASSERT_TRUE(mFilterTests.stopFilter(filterId));
    ASSERT_TRUE(mDvrTests.stopDvr());
    ASSERT_TRUE(mFilterTests.closeFilter(filterId));
    mDvrTests.closeDvr();
    ASSERT_TRUE(mDemuxTests.closeDemux());
}

void TunerRecordHidlTest::recordSingleFilterTest(FilterConfig filterConf,
                                                 FrontendConfig frontendConf, DvrConfig dvrConf) {
    uint32_t feId;
    uint32_t demuxId;
    sp<IDemux> demux;
    uint32_t filterId;
    sp<IFilter> filter;

    mFrontendTests.getFrontendIdByType(frontendConf.type, feId);
    ASSERT_TRUE(feId != INVALID_ID);
    ASSERT_TRUE(mFrontendTests.openFrontendById(feId));
    ASSERT_TRUE(mFrontendTests.setFrontendCallback());
    ASSERT_TRUE(mDemuxTests.openDemux(demux, demuxId));
    ASSERT_TRUE(mDemuxTests.setDemuxFrontendDataSource(feId));
    mFilterTests.setDemux(demux);
    mDvrTests.setDemux(demux);
    ASSERT_TRUE(mDvrTests.openDvrInDemux(dvrConf.type, dvrConf.bufferSize));
    ASSERT_TRUE(mDvrTests.configDvr(dvrConf.settings));
    ASSERT_TRUE(mDvrTests.getDvrMQDescriptor());
    ASSERT_TRUE(mFilterTests.openFilterInDemux(filterConf.type, filterConf.bufferSize));
    ASSERT_TRUE(mFilterTests.getNewlyOpenedFilterId(filterId));
    ASSERT_TRUE(mFilterTests.configFilter(filterConf.settings, filterId));
    ASSERT_TRUE(mFilterTests.getFilterMQDescriptor(filterId));
    filter = mFilterTests.getFilterById(filterId);
    ASSERT_TRUE(filter != nullptr);
    mDvrTests.startRecordOutputThread(dvrConf.settings.record());
    ASSERT_TRUE(mDvrTests.attachFilterToDvr(filter));
    ASSERT_TRUE(mDvrTests.startDvr());
    ASSERT_TRUE(mFilterTests.startFilter(filterId));
    ASSERT_TRUE(mFrontendTests.tuneFrontend(frontendConf));
    mDvrTests.testRecordOutput();
    mDvrTests.stopRecordThread();
    ASSERT_TRUE(mFrontendTests.stopTuneFrontend());
    ASSERT_TRUE(mFilterTests.stopFilter(filterId));
    ASSERT_TRUE(mDvrTests.stopDvr());
    ASSERT_TRUE(mDvrTests.detachFilterToDvr(filter));
    ASSERT_TRUE(mFilterTests.closeFilter(filterId));
    mDvrTests.closeDvr();
    ASSERT_TRUE(mDemuxTests.closeDemux());
    ASSERT_TRUE(mFrontendTests.closeFrontend());
}

void TunerRecordHidlTest::attachSingleFilterToRecordDvrTest(FilterConfig filterConf,
                                                            FrontendConfig frontendConf,
                                                            DvrConfig dvrConf) {
    uint32_t feId;
    uint32_t demuxId;
    sp<IDemux> demux;
    uint32_t filterId;
    sp<IFilter> filter;

    mFrontendTests.getFrontendIdByType(frontendConf.type, feId);
    ASSERT_TRUE(feId != INVALID_ID);
    ASSERT_TRUE(mFrontendTests.openFrontendById(feId));
    ASSERT_TRUE(mFrontendTests.setFrontendCallback());
    ASSERT_TRUE(mDemuxTests.openDemux(demux, demuxId));
    ASSERT_TRUE(mDemuxTests.setDemuxFrontendDataSource(feId));
    mFilterTests.setDemux(demux);
    mDvrTests.setDemux(demux);
    ASSERT_TRUE(mDvrTests.openDvrInDemux(dvrConf.type, dvrConf.bufferSize));
    ASSERT_TRUE(mDvrTests.configDvr(dvrConf.settings));
    ASSERT_TRUE(mDvrTests.getDvrMQDescriptor());
    ASSERT_TRUE(mFilterTests.openFilterInDemux(filterConf.type, filterConf.bufferSize));
    ASSERT_TRUE(mFilterTests.getNewlyOpenedFilterId(filterId));
    ASSERT_TRUE(mFilterTests.configFilter(filterConf.settings, filterId));
    ASSERT_TRUE(mFilterTests.getFilterMQDescriptor(filterId));
    filter = mFilterTests.getFilterById(filterId);
    ASSERT_TRUE(filter != nullptr);
    ASSERT_TRUE(mDvrTests.attachFilterToDvr(filter));
    ASSERT_TRUE(mDvrTests.startDvr());
    ASSERT_TRUE(mFilterTests.startFilter(filterId));
    ASSERT_TRUE(mFilterTests.stopFilter(filterId));
    ASSERT_TRUE(mDvrTests.stopDvr());
    ASSERT_TRUE(mDvrTests.detachFilterToDvr(filter));
    ASSERT_TRUE(mFilterTests.closeFilter(filterId));
    mDvrTests.closeDvr();
    ASSERT_TRUE(mDemuxTests.closeDemux());
    ASSERT_TRUE(mFrontendTests.closeFrontend());
}

void TunerDescramblerHidlTest::scrambledBroadcastTest(set<struct FilterConfig> mediaFilterConfs,
                                                      FrontendConfig frontendConf,
                                                      DescramblerConfig descConfig) {
    uint32_t feId;
    uint32_t demuxId;
    sp<IDemux> demux;
    set<uint32_t> filterIds;
    uint32_t filterId;
    set<struct FilterConfig>::iterator config;
    set<uint32_t>::iterator id;

    mFrontendTests.getFrontendIdByType(frontendConf.type, feId);
    if (feId == INVALID_ID) {
        // TODO broadcast test on Cuttlefish needs licensed ts input,
        // these tests are runnable on vendor device with real frontend module
        // or with manual ts installing and use DVBT frontend.
        return;
    }
    ASSERT_TRUE(mFrontendTests.openFrontendById(feId));
    ASSERT_TRUE(mFrontendTests.setFrontendCallback());
    ASSERT_TRUE(mDemuxTests.openDemux(demux, demuxId));
    ASSERT_TRUE(mDemuxTests.setDemuxFrontendDataSource(feId));
    mFilterTests.setDemux(demux);
    for (config = mediaFilterConfs.begin(); config != mediaFilterConfs.end(); config++) {
        ASSERT_TRUE(mFilterTests.openFilterInDemux((*config).type, (*config).bufferSize));
        ASSERT_TRUE(mFilterTests.getNewlyOpenedFilterId(filterId));
        ASSERT_TRUE(mFilterTests.configFilter((*config).settings, filterId));
        filterIds.insert(filterId);
    }
    mDescramblerTests.openDescrambler(demuxId);
    TunerKeyToken token;
    ASSERT_TRUE(mDescramblerTests.getKeyToken(descConfig.casSystemId, descConfig.provisionStr,
                                              descConfig.hidlPvtData, token));
    mDescramblerTests.setKeyToken(token);
    vector<DemuxPid> pids;
    DemuxPid pid;
    for (config = mediaFilterConfs.begin(); config != mediaFilterConfs.end(); config++) {
        ASSERT_TRUE(mDescramblerTests.getDemuxPidFromFilterSettings((*config).type,
                                                                    (*config).settings, pid));
        pids.push_back(pid);
        mDescramblerTests.addPid(pid, nullptr);
    }
    for (id = filterIds.begin(); id != filterIds.end(); id++) {
        ASSERT_TRUE(mFilterTests.startFilter(*id));
    }
    // tune test
    ASSERT_TRUE(mFrontendTests.tuneFrontend(frontendConf));
    ASSERT_TRUE(filterDataOutputTest(goldenOutputFiles));
    ASSERT_TRUE(mFrontendTests.stopTuneFrontend());
    for (id = filterIds.begin(); id != filterIds.end(); id++) {
        ASSERT_TRUE(mFilterTests.stopFilter(*id));
    }
    for (auto pid : pids) {
        mDescramblerTests.removePid(pid, nullptr);
    }
    mDescramblerTests.closeDescrambler();
    for (id = filterIds.begin(); id != filterIds.end(); id++) {
        ASSERT_TRUE(mFilterTests.closeFilter(*id));
    }
    ASSERT_TRUE(mDemuxTests.closeDemux());
    ASSERT_TRUE(mFrontendTests.closeFrontend());
}

TEST_P(TunerFrontendHidlTest, TuneFrontend) {
    description("Tune one Frontend with specific setting and check Lock event");
    mFrontendTests.tuneTest(frontendArray[DVBT]);
}

TEST_P(TunerFrontendHidlTest, AutoScanFrontend) {
    description("Run an auto frontend scan with specific setting and check lock scanMessage");
    mFrontendTests.scanTest(frontendScanArray[SCAN_DVBT], FrontendScanType::SCAN_AUTO);
}

TEST_P(TunerFrontendHidlTest, BlindScanFrontend) {
    description("Run an blind frontend scan with specific setting and check lock scanMessage");
    mFrontendTests.scanTest(frontendScanArray[SCAN_DVBT], FrontendScanType::SCAN_BLIND);
}

TEST_P(TunerDemuxHidlTest, openDemux) {
    description("Open and close a Demux.");
    uint32_t feId;
    uint32_t demuxId;
    sp<IDemux> demux;
    mFrontendTests.getFrontendIdByType(frontendArray[DVBT].type, feId);
    ASSERT_TRUE(feId != INVALID_ID);
    ASSERT_TRUE(mFrontendTests.openFrontendById(feId));
    ASSERT_TRUE(mFrontendTests.setFrontendCallback());
    ASSERT_TRUE(mDemuxTests.openDemux(demux, demuxId));
    ASSERT_TRUE(mDemuxTests.setDemuxFrontendDataSource(feId));
    ASSERT_TRUE(mDemuxTests.closeDemux());
    ASSERT_TRUE(mFrontendTests.closeFrontend());
}

TEST_P(TunerFilterHidlTest, StartFilterInDemux) {
    description("Open and start a filter in Demux.");
    // TODO use paramterized tests
    configSingleFilterInDemuxTest(filterArray[TS_VIDEO0], frontendArray[DVBT]);
}

TEST_P(TunerBroadcastHidlTest, BroadcastDataFlowVideoFilterTest) {
    description("Test Video Filter functionality in Broadcast use case.");
    broadcastSingleFilterTest(filterArray[TS_VIDEO1], frontendArray[DVBS]);
}

TEST_P(TunerBroadcastHidlTest, BroadcastDataFlowAudioFilterTest) {
    description("Test Audio Filter functionality in Broadcast use case.");
    broadcastSingleFilterTest(filterArray[TS_AUDIO0], frontendArray[DVBS]);
}

TEST_P(TunerBroadcastHidlTest, BroadcastDataFlowSectionFilterTest) {
    description("Test Section Filter functionality in Broadcast use case.");
    broadcastSingleFilterTest(filterArray[TS_SECTION0], frontendArray[DVBS]);
}

TEST_P(TunerBroadcastHidlTest, IonBufferTest) {
    description("Test the av filter data bufferring.");
    broadcastSingleFilterTest(filterArray[TS_VIDEO0], frontendArray[DVBS]);
}

TEST_P(TunerPlaybackHidlTest, PlaybackDataFlowWithTsSectionFilterTest) {
    description("Feed ts data from playback and configure Ts section filter to get output");
    playbackSingleFilterTest(filterArray[TS_SECTION0], dvrArray[DVR_PLAYBACK0]);
}

TEST_P(TunerRecordHidlTest, AttachFiltersToRecordTest) {
    description("Attach a single filter to the record dvr test.");
    // TODO use paramterized tests
    attachSingleFilterToRecordDvrTest(filterArray[TS_RECORD0], frontendArray[DVBT],
                                      dvrArray[DVR_RECORD0]);
}

TEST_P(TunerRecordHidlTest, RecordDataFlowWithTsRecordFilterTest) {
    description("Feed ts data from frontend to recording and test with ts record filter");
    recordSingleFilterTest(filterArray[TS_RECORD0], frontendArray[DVBT], dvrArray[DVR_RECORD0]);
}

TEST_P(TunerDescramblerHidlTest, CreateDescrambler) {
    description("Create Descrambler");
    uint32_t feId;
    uint32_t demuxId;
    sp<IDemux> demux;
    mFrontendTests.getFrontendIdByType(frontendArray[DVBT].type, feId);
    ASSERT_TRUE(feId != INVALID_ID);
    ASSERT_TRUE(mFrontendTests.openFrontendById(feId));
    ASSERT_TRUE(mFrontendTests.setFrontendCallback());
    ASSERT_TRUE(mDemuxTests.openDemux(demux, demuxId));
    ASSERT_TRUE(mDemuxTests.setDemuxFrontendDataSource(feId));
    mDescramblerTests.openDescrambler(demuxId);
    mDescramblerTests.closeDescrambler();
    ASSERT_TRUE(mDemuxTests.closeDemux());
    ASSERT_TRUE(mFrontendTests.closeFrontend());
}

TEST_P(TunerDescramblerHidlTest, ScrambledBroadcastDataFlowMediaFiltersTest) {
    description("Test ts audio filter in scrambled broadcast use case");
    set<FilterConfig> filterConfs;
    filterConfs.insert(filterArray[TS_AUDIO0]);
    filterConfs.insert(filterArray[TS_VIDEO1]);
    scrambledBroadcastTest(filterConfs, frontendArray[DVBT], descramblerArray[DESC_0]);
}

/*INSTANTIATE_TEST_SUITE_P(
        PerInstance, TunerFrontendHidlTest,
        testing::ValuesIn(android::hardware::getAllHalInstanceNames(ITuner::descriptor)),
        android::hardware::PrintInstanceNameToString);

INSTANTIATE_TEST_SUITE_P(
        PerInstance, TunerDemuxHidlTest,
        testing::ValuesIn(android::hardware::getAllHalInstanceNames(ITuner::descriptor)),
        android::hardware::PrintInstanceNameToString);

INSTANTIATE_TEST_SUITE_P(
        PerInstance, TunerFilterHidlTest,
        testing::ValuesIn(android::hardware::getAllHalInstanceNames(ITuner::descriptor)),
        android::hardware::PrintInstanceNameToString);

INSTANTIATE_TEST_SUITE_P(
        PerInstance, TunerBroadcastHidlTest,
        testing::ValuesIn(android::hardware::getAllHalInstanceNames(ITuner::descriptor)),
        android::hardware::PrintInstanceNameToString);

INSTANTIATE_TEST_SUITE_P(
        PerInstance, TunerPlaybackHidlTest,
        testing::ValuesIn(android::hardware::getAllHalInstanceNames(ITuner::descriptor)),
        android::hardware::PrintInstanceNameToString);

INSTANTIATE_TEST_SUITE_P(
        PerInstance, TunerRecordHidlTest,
        testing::ValuesIn(android::hardware::getAllHalInstanceNames(ITuner::descriptor)),
        android::hardware::PrintInstanceNameToString);*/

INSTANTIATE_TEST_SUITE_P(
        PerInstance, TunerDescramblerHidlTest,
        testing::ValuesIn(android::hardware::getAllHalInstanceNames(ITuner::descriptor)),
        android::hardware::PrintInstanceNameToString);
}  // namespace
