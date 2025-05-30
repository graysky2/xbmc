/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "utils/Variant.h"

#include <string>

class CFileItemList;
class CProfileManager;
class CURL;
class CFileItem;

namespace XFILE
{
  enum DIR_CACHE_TYPE
  {
    DIR_CACHE_NEVER = 0, ///< Never cache this directory to memory
    DIR_CACHE_ONCE,      ///< Cache this directory to memory for each fetch (so that FileExists() checks are fast)
    DIR_CACHE_ALWAYS     ///< Always cache this directory to memory, so that each additional fetch of this folder will utilize the cache (until it's cleared)
  };

  /*! \brief Available directory flags
   The defaults are to allow file directories, no prompting, retrieve file information, hide hidden files, and utilise the directory cache
   based on the implementation's wishes.
   */
  enum DIR_FLAG
  {
    DIR_FLAG_DEFAULTS      = 0,
    DIR_FLAG_NO_FILE_DIRS  = (2 << 0), ///< Don't convert files (zip, rar etc.) to directories
    DIR_FLAG_ALLOW_PROMPT  = (2 << 1), ///< Allow prompting for further info (passwords etc.)
    DIR_FLAG_NO_FILE_INFO  = (2 << 2), ///< Don't read additional file info (stat for example)
    DIR_FLAG_GET_HIDDEN    = (2 << 3), ///< Get hidden files
    DIR_FLAG_READ_CACHE    = (2 << 4), ///< Force reading from the directory cache (if available)
    DIR_FLAG_BYPASS_CACHE  = (2 << 5)  ///< Completely bypass the directory cache (no reading, no writing)
  };
/*!
 \ingroup filesystem
 \brief Interface to the directory on a file system.

 This Interface is retrieved from CDirectoryFactory and can be used to
 access the directories on a filesystem.
 \sa CDirectoryFactory
 */
class IDirectory
{
public:
  static void RegisterProfileManager(const CProfileManager &profileManager);
  static void UnregisterProfileManager();

  IDirectory();
  virtual ~IDirectory(void);
  /*!
   \brief Get the \e items of the directory \e strPath.
   \param url Directory to read.
   \param items Retrieves the directory entries.
   \return Returns \e true, if successful.
   \sa CDirectoryFactory
   */
  virtual bool GetDirectory(const CURL& url, CFileItemList &items) = 0;
  /*!
   \brief Retrieve the progress of the current directory fetch (if possible).
   \return the progress as a float in the range 0..100.
   \sa GetDirectory, CancelDirectory
   */
  virtual float GetProgress() const { return 0.0f; }
  /*!
   \brief Cancel the current directory fetch (if possible).
   \sa GetDirectory
   */
  virtual void CancelDirectory() {}
  /*!
  \brief Create the directory
  \param url Directory to create.
  \return Returns \e true, if directory is created or if it already exists
  \sa CDirectoryFactory
  */
  virtual bool Create(const CURL& url) { return false; }
  /*!
  \brief Check for directory existence
  \param url Directory to check.
  \return Returns \e true, if directory exists
  \sa CDirectoryFactory
  */
  virtual bool Exists(const CURL& url) { return false; }
  /*!
  \brief Removes the directory
  \param url Directory to remove.
  \return Returns \e false if not successful
  */
  virtual bool Remove(const CURL& url) { return false; }

  /*!
  \brief Provided a path, attempts to resolve to a mount point
  \param path Path to resolve
  \return Returns the mountpoint if found, else the provided path
  */
  virtual std::string ResolveMountPoint(const std::string& path) const { return path; }

  /*!
  \brief Recursively removes the directory
  \param url Directory to remove.
  \return Returns \e false if not successful
  */
  virtual bool RemoveRecursive(const CURL& url) { return false; }

  /*!
  \brief Whether this file should be listed
  \param url File to test.
  \return Returns \e true if the file should be listed
  */
  virtual bool IsAllowed(const CURL& url) const;

  /*! \brief Whether to allow all files/folders to be listed.
   \return Returns \e true if all files/folder should be listed.
   */
  virtual bool AllowAll() const { return false; }

  /*!
  \brief How this directory should be cached
  \param url Directory at hand.
  \return Returns the cache type.
  */
  virtual DIR_CACHE_TYPE GetCacheType(const CURL& url) const { return DIR_CACHE_ONCE; }

  void SetMask(const std::string& strMask);
  void SetFlags(int flags);

  /*! \brief Process additional requirements before the directory fetch is performed.
   Some directory fetches may require authentication, keyboard input etc.  The IDirectory subclass
   should call GetKeyboardInput, SetErrorDialog or RequireAuthentication and then return false
   from the GetDirectory method. CDirectory will then prompt for input from the user, before
   re-calling the GetDirectory method.
   \sa GetKeyboardInput, SetErrorDialog, RequireAuthentication
   */
  bool ProcessRequirements();

  /*!
  \brief Resolves a given item to a playable item
  \note Some directories (e.g. dvd, bluray, plugins etc) need to be translated/resolved to the actual playback url
  \param item The item being manipulated (which the path points to a vfs protocol implementation)
  \return true if the item was resolved, false if it failed to resolve
  */
  virtual bool Resolve(CFileItem& item) const { return true; };

protected:
  /*! \brief Prompt the user for some keyboard input
   Call this method from the GetDirectory method to retrieve additional input from the user.
   If this function returns false then no input has been received, and the GetDirectory call
   should return false.
   \param heading an integer or string heading for the keyboard dialog
   \param input [out] the returned input (if available).
   \return true if keyboard input has been received. False if it hasn't.
   \sa ProcessRequirements
   */
  bool GetKeyboardInput(const CVariant &heading, std::string &input, bool hiddenInput = false);

  /*! \brief Show an error dialog on failure of GetDirectory call
   Call this method from the GetDirectory method to set an error message to be shown to the user
   \param heading an integer or string heading for the error dialog.
   \param line1 the first line to be displayed (integer or string).
   \param line2 the first line to be displayed (integer or string).
   \param line3 the first line to be displayed (integer or string).
   \sa ProcessRequirements
   */
  void SetErrorDialog(const CVariant &heading, const CVariant &line1, const CVariant &line2 = 0, const CVariant &line3 = 0);

  /*! \brief Prompt the user for authentication of a URL.
   Call this method from the GetDirectory method when authentication is required from the user, before returning
   false from the GetDirectory call. The user will be prompted for authentication, and GetDirectory will be
   re-called.
   \param url the URL to authenticate.
   \sa ProcessRequirements
   */
  void RequireAuthentication(const CURL& url);

  static const CProfileManager *m_profileManager;

  std::string m_strFileMask;  ///< Holds the file mask specified by SetMask()

  int m_flags; ///< Directory flags - see DIR_FLAG

  CVariant m_requirements;
};
}
