/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
#if !defined(MediaOmxReader_h_)
#define MediaOmxReader_h_

#include "MediaResource.h"
#include "MediaDecoderReader.h"
#include "nsRect.h"
#include "mozilla/dom/AudioChannelBinding.h"
#include <ui/GraphicBuffer.h>
#include <stagefright/MediaSource.h>

namespace android {
class OmxDecoder;
class MOZ_EXPORT MediaExtractor;
}

namespace mozilla {

namespace dom {
  class TimeRanges;
}

class AbstractMediaDecoder;

class MediaOmxReader : public MediaDecoderReader
{
  nsCString mType;
  bool mHasVideo;
  bool mHasAudio;
  nsIntRect mPicture;
  nsIntSize mInitialFrame;
  int64_t mVideoSeekTimeUs;
  int64_t mAudioSeekTimeUs;
  int32_t mSkipCount;
  dom::AudioChannel mAudioChannel;
  android::sp<android::MediaSource> mAudioOffloadTrack;

protected:
  android::sp<android::OmxDecoder> mOmxDecoder;
  android::sp<android::MediaExtractor> mExtractor;


  // A cache value updated by UpdateIsWaitingMediaResources(), makes the
  // "waiting resources state" is synchronous to StateMachine.
  bool mIsWaitingResources;

  // Called by ReadMetadata() during MediaDecoderStateMachine::DecodeMetadata()
  // on decode thread. It create and initialize the OMX decoder including
  // setting up custom extractor. The extractor provide the essential
  // information used for creating OMX decoder such as video/audio codec.
  virtual nsresult InitOmxDecoder();

  // Called inside DecodeVideoFrame, DecodeAudioData, ReadMetadata and Seek
  // to activate the decoder automatically.
  virtual void EnsureActive();

  // Check the underlying HW resources are available and store the result in
  // mIsWaitingResources. The result might be changed by binder thread,
  // Can only called by ReadMetadata.
  void UpdateIsWaitingMediaResources();

public:
  MediaOmxReader(AbstractMediaDecoder* aDecoder);
  ~MediaOmxReader();

  virtual nsresult Init(MediaDecoderReader* aCloneDonor);

  virtual void NotifyDataArrived(const char* aBuffer, uint32_t aLength, int64_t aOffset);

  virtual bool DecodeAudioData();
  virtual bool DecodeVideoFrame(bool &aKeyframeSkip,
                                int64_t aTimeThreshold);

  virtual bool HasAudio()
  {
    return mHasAudio;
  }

  virtual bool HasVideo()
  {
    return mHasVideo;
  }

  // Return mIsWaitingResources.
  virtual bool IsWaitingMediaResources() MOZ_OVERRIDE;

  virtual bool IsDormantNeeded();
  virtual void ReleaseMediaResources();

  virtual void ReleaseDecoder() MOZ_OVERRIDE;

  virtual void PreReadMetadata() MOZ_OVERRIDE;
  virtual nsresult ReadMetadata(MediaInfo* aInfo,
                                MetadataTags** aTags);
  virtual nsresult Seek(int64_t aTime, int64_t aStartTime, int64_t aEndTime, int64_t aCurrentTime);

  virtual void SetIdle() MOZ_OVERRIDE;

  void SetAudioChannel(dom::AudioChannel aAudioChannel) {
    mAudioChannel = aAudioChannel;
  }

  android::sp<android::MediaSource> GetAudioOffloadTrack() {
    return mAudioOffloadTrack;
  }

#ifdef MOZ_AUDIO_OFFLOAD
  // Check whether it is possible to offload current audio track. This access
  // canOffloadStream() from libStageFright Utils.cpp, which is not there in
  // ANDROID_VERSION < 19
  void CheckAudioOffload();
#endif
};

} // namespace mozilla

#endif
