FROM openjdk:8-jdk

RUN apt-get update \
    && apt-get install -y build-essential ruby git libonig-dev libbison-dev libncurses5

ENV SDK_TOOL_VERSION=sdk-tools-linux-3859397
ENV NDK_VERSION=android-ndk-r13b-linux-x86_64
ENV ANDROID_HOME=/opt/android-sdk
ENV ANDROID_NDK_HOME=/opt/android-ndk-r13b
ENV PATH=$PATH:$ANDROID_HOME/tools:$ANDROID_HOME/tools/bin:$ANDROID_HOME/platform-tools

RUN mkdir $ANDROID_HOME \
    && wget "https://dl.google.com/android/repository/${SDK_TOOL_VERSION}.zip" \
    && unzip -d $ANDROID_HOME ${SDK_TOOL_VERSION}.zip \
    && rm -rf ${SDK_TOOL_VERSION}.zip

RUN mkdir $ANDROID_NDK_HOME \
    && wget "https://dl.google.com/android/repository/${NDK_VERSION}.zip" \
    && unzip -d /opt ${NDK_VERSION}.zip \
    && rm -rf ${NDK_VERSION}.zip

RUN mkdir ~/.android \
    && touch ~/.android/repositories.cfg
RUN yes | sdkmanager --licenses

RUN sdkmanager \
    "tools" \
    "platform-tools" \
    "extras;android;m2repository" \
    "build-tools;28.0.3" \
    "platforms;android-28" \
    "cmake;3.6.4111459"
