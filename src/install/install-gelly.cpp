#include "install-gelly.hpp"

#include "helpers/find-gmod-directory.hpp"
#include "helpers/find-steam-directory.hpp"

#include <fstream>
#include <zip.h>

namespace gelly {
namespace {
void CleanOldInstallation(const GellyInstallation &installation) {
  remove_all(installation.addonPath);
  remove(installation.binaryModulePath);
}

struct ReleaseInfo {
  zip_source_t *source;
  std::shared_ptr<Response> response;

  bool IsEmpty() const { return !source; }
};

ReleaseInfo DownloadRelease(const std::string &url,
                            const std::shared_ptr<Curl> &curl) {
  const auto response = curl->Get(url);
  if (response->responseCode != 200) {
    return {nullptr, nullptr};
  }

  const auto source = zip_source_buffer_create(
      response->data->data(), response->data->size(), 0, nullptr);

  if (!source) {
    return {nullptr, nullptr};
  }

  return {source, response};
}

void ExtractReleaseToDir(const std::filesystem::path &dir,
                         const ReleaseInfo &release) {
  auto *archive = zip_open_from_source(release.source, 0, nullptr);
  if (!archive) {
    throw std::runtime_error("Failed to open zip archive.");
  }

  try {
    zip_int64_t numFiles = zip_get_num_files(archive);
    for (zip_int64_t i = 0; i < numFiles; i++) {
      auto *file = zip_fopen_index(archive, i, 0);
      if (!file) {
        throw std::runtime_error("Failed to open file in zip archive.");
      }

      zip_stat_t stat;
      zip_stat_init(&stat);

      // we must specify zero for the flags parameter, it's not a default
      // argument, zero gives us an absolute path from the archive to the file
      if (zip_stat_index(archive, i, 0, &stat) != 0) {
        throw std::runtime_error("Failed to get file info from zip archive.");
      }

      const auto path = dir / stat.name;
      auto buffer = std::vector<char>(stat.size);
      const auto bytesRead = zip_fread(
          file, reinterpret_cast<void *>(buffer.data()), buffer.size());

      if (bytesRead != stat.size) {
        throw std::runtime_error("Failed to read file from zip archive.");
      }

      std::ofstream out(path, std::ios::binary);
      out.write(buffer.data(), buffer.size());
      out.close();

      zip_fclose(file);
    }
  } catch (...) {
    zip_discard(archive);
    throw;
  }

  zip_discard(archive);
}
} // namespace

void InstallGelly(const LatestGellyInfo &info,
                  const std::shared_ptr<Curl> &curl,
                  std::optional<GellyInstallation> priorInstallation) {
  if (priorInstallation.has_value()) {
    CleanOldInstallation(priorInstallation.value());
  }

  const auto release = DownloadRelease(info.downloadUrl, curl);
  if (release.IsEmpty()) {
    throw std::runtime_error("Failed to download Gelly release.");
  }

  const auto gmodDir =
      helpers::FindGModDirectory(helpers::FindSteamDirectory().value());
  if (!gmodDir.has_value()) {
    throw std::runtime_error("Failed to find Garry's Mod directory.");
  }

  ExtractReleaseToDir(*gmodDir, release);
}

} // namespace gelly