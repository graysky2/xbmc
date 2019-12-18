/*
 *  Copyright (C) 2005-2021 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "windowing/VideoSync.h"
#include "guilib/DispResource.h"

class CVideoSyncGbm : public CVideoSync, IDispResource
{
public:
  CVideoSyncGbm(void *clock) : CVideoSync(clock) {};
  virtual bool Setup(PUPDATECLOCK func);
  virtual void Run(CEvent& stopEvent);
  virtual void Cleanup();
  virtual float GetFps();
  virtual void OnResetDisplay();
  virtual void RefreshChanged();

private:
  int m_fd = -1;
  uint32_t m_crtcId = 0;
  uint64_t m_sequence = 0;
  uint64_t m_offset = 0;
  volatile bool m_abort;
};
