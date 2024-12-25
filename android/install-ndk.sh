CMDTOOLS_LINK='https://dl.google.com/android/repository/commandlinetools-linux-11076708_latest.zip'
SDK_LOC="$HOME/android-sdk"
CMDTOOLS_LOC="$SDK_LOC/tools/latest"

cd $HOME

wget $CMDTOOLS_LINK

unzip commandlinetools*.zip
rm commandlinetools*.zip

mkdir -p $SDK_LOC
mkdir -p $CMDTOOLS_LOC

mv cmdline-tools $CMDTOOLS_LOC

echo "export ANDROID_SDK_ROOT=$SDK_LOC" >> $HOME/.bashrc
echo "export ANDROID_HOME=$SDK_LOC" >> $HOME/.bashrc
echo "export PATH=$PATH:$CMDTOOLS_LOC/cmdline-tools/bin" >> $HOME/.bashrc

source $HOME/.bashrc

run_sdk() {
    sdkmanager --sdk_root=$SDK_LOC $*
}

run_sdk --list
run_sdk "ndk-bundle"
run_sdk "cmake;3.10.2.4988404"