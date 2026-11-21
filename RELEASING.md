# Making a release

Releases are built by the Release workflow (`.github/workflows/release.yml`).

## One-time setup: Android signing key

The Android APK on GitHub is signed with a key used only for GitHub downloads. It is separate from the Google Play key, so the Play key never goes into CI. Do this once, on a machine with a JDK or Android Studio.

1. Create the key. `keytool` comes with any JDK, and with Android Studio (in `C:\Program Files\Android\Android Studio\jbr\bin`).

   ```powershell
   keytool -genkeypair -v -keystore picasim-github.jks -storetype PKCS12 -alias picasim-github -keyalg RSA -keysize 4096 -validity 10000 -dname "CN=PicaSim GitHub releases"
   ```

   It asks for a password. In a PKCS12 keystore the key uses the same password.

2. Back up `picasim-github.jks` and the password somewhere safe, away from this repo (a password manager, for example). If the key is lost, people who installed the GitHub APK will have to uninstall before they can install a build signed with a new key. Never commit the keystore.

3. On GitHub, go to the repo's Settings > Environments > New environment, and name it `android-release`.
   - Tick "Required reviewers" and add yourself, so the signing job waits for your approval every time.
   - Optionally, under "Deployment branches and tags", limit it to `main`. Dry runs from other branches then can't sign the APK.

4. In that environment, add four environment secrets:
   - `ANDROID_KEYSTORE_BASE64`: the keystore as base64. This copies it to the clipboard:
     ```powershell
     [Convert]::ToBase64String([IO.File]::ReadAllBytes("$PWD\picasim-github.jks")) | Set-Clipboard
     ```
   - `ANDROID_KEYSTORE_PASSWORD`: the password
   - `ANDROID_KEY_ALIAS`: `picasim-github`
   - `ANDROID_KEY_PASSWORD`: the password again

   Copy something else afterwards so the keystore isn't left on the clipboard.

## Each release

1. Add the new version and its changes to the top of `VERSIONS.txt`, and merge that to `main`. The first line sets the version for every platform.
2. Go to Actions > Release > Run workflow, and run it on `main`.
3. Approve the `android-release` deployment when GitHub asks.
4. When it finishes, a draft release `v<version>` is on the Releases page with the build files attached. Write the release notes there.
5. Publish the draft. This also creates the `v<version>` tag.

Running the workflow on any other branch is a dry run. It builds everything and uploads the files as a `picasim-release-files` artifact, without making a release.

## Notes

- The macOS DMG from CI is not notarized. The README describes how to make a notarized DMG locally.
- iOS builds go out through TestFlight (see `ios_archive.sh`), not GitHub releases.
- The Android job's "Check signature" step logs the certificate fingerprint, which can be quoted in the release notes so people can check the APK.
