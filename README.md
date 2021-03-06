# NDK Project

参考NDK例子native-activity，含有比较简单的工程结构，既能cygwin下ndk-build（无IDE），也可以作为Android工程导入（使用IDE）

1. copy android-ndk-r10/samples/native-activity and remove or replace main.c, to get a ndk project
2. code and modify Android.mk to your module, note APP_ABI armeabi-v7a x86 (Application.mk)

3. $ ndk-build NDK_PROJECT_PATH=/cygdrive/f/Android/android-ndk-r10/samples/z_ndk_workspace/messagepack
4. $ adb push 'F:\Android\android-ndk-r10\samples\z_ndk_workspace\messagepack\libs\armeabi-v7a\cmp_unpack' /data/local/tmp
5. $ adb shell chmod 6755 /data/local/tmp/cmp_unpack
6. $ adb shell a /data/local/tmp/cmp_unpack
7. OK.

$ ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk

NDK_PROJECT_PATH 指定了需要编译的代码的工程目录，这里给出的是当前目录，APP_BUILD_SCRIPT给出的是Android makefile文件的路径，当然，如果你还有 Application.mk 文件的话，则可以添加 NDK_APP_APPLICATION_MK=./Application.mk 

# a ndk-build problem
* question
```
$ ndk-build
Android NDK: Could not find application project directory !    
Android NDK: Please define the NDK_PROJECT_PATH variable to point to it.    
/opt/android-ndk-r10b/build/core/build-local.mk:148: *** Android NDK: Aborting    .  Stop.
```
* answer
```
You need to specify 3 things.
NDK_PROJECT_PATH - the location of your project
NDK_APPLICATION_MK - the path of the Application.mk file
APP_BUILD_SCRIPT - the path to the Android.mk file

These are needed to override the default values of the build script, which expects things to be in the jni folder.

When calling ndk-build use
ndk-build NDK_PROJECT_PATH=/path/to/proj NDK_APPLICATION_MK=/path/to/Application.mk

In Application.mk add
APP_BUILD_SCRIPT := /path/to/Android.mk
```

include $(call all-makefiles-under,$(LOCAL_PATH)) 对$(LOCAL_PATH)指定目录下所有子孙目录的Android.mk  
include $(call all-subdir-under) 对当前目录下所有一级子目录Android.mk（如果没有类似递归语法）  

[源码](http://androidxref.com/4.2_r1/xref/ndk/build/core/definitions.mk)
```mk
# -----------------------------------------------------------------------------
# Macro    : my-dir
# Returns  : the directory of the current Makefile
# Usage    : $(my-dir)
# -----------------------------------------------------------------------------
my-dir = $(call parent-dir,$(lastword $(MAKEFILE_LIST)))

# -----------------------------------------------------------------------------
# Function : all-makefiles-under
# Arguments: 1: directory path
# Returns  : a list of all makefiles immediately below some directory
# Usage    : $(call all-makefiles-under, <some path>)
# -----------------------------------------------------------------------------
all-makefiles-under = $(wildcard $1/*/Android.mk)

# -----------------------------------------------------------------------------
# Macro    : all-subdir-makefiles
# Returns  : list of all makefiles in subdirectories of the current Makefile's
#            location
# Usage    : $(all-subdir-makefiles)
# -----------------------------------------------------------------------------
all-subdir-makefiles = $(call all-makefiles-under,$(call my-dir))
```

VistualGDB

#VS+VA

(参考)[http://bbs.pediy.com/showthread.php?p=1353066]

打开VS2008，新建Makefile项目

环境变量：

将NDK根目录加入%PATH%环境变量，这样直接使用命令ndk-build。

在项目向导中填写以下内容

* Build command line: ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk
* Clean commands: ndk-build clean NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk
* Rebuild command line: ndk-build -B NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk
* Include search path: E:\Android\android-ndk-r9d\platforms\android-12\arch-arm\usr\include // 对应到你本机的目录及版本。
点击完成。

#使用makefile的库

一般Android写c/c++的NDK使用Android.mk。当使用其他开源库时，大多是通过Makefile编译的，而又可能再使用其它开源库，如果再写Android.mk会很麻烦。虽然一些可以在AOSP中可以找到。这些其实和toolchain有关。故可以独立出一个toolchain，直接使用Makefile，而不需要自己去重新编写Android.mk文件，减少了很多麻烦。

从Android NDK中独立toolchain步骤（系统为Ubuntu(32位)）：

下载Android NDK
独立toolchain

把NDK压缩包解压到系统，如/mnt目录下，后在/mnt目录下建立文件夹my_ndk_toolchain，然后再/mnt目录下执行以下命令：

/mnt/android-ndk-r9c/build/tools/make-standalone-toolchain.sh --platform=android-19 --toolchain=arm-linux-androideabi-4.8 --stl=stlport --install-dir=/mnt/my_ndk_toolchain

出现以下打印：

dir=/mnt/my_ndk_toolchain  
Copying prebuilt binaries...  
Copying sysroot headers and libraries...  
Copying libstdc++ headers and libraries...  
Copying files to: /mnt/my_ndk_toolchain  
Cleaning up...  
Done.  

说明独立的工具链成功，对执行的命令进行简单说明：

* /mnt/android-ndk-r9c/build/tools/make-standalone-toolchain.sh：执行NDK目录下make-standalone-toolchain.sh脚本；
* --platform：指工具链将使用哪个版本的Android API，可cd /mnt/android-ndk-r9c/platform中查看，我这里使用的是Android-19；
* --toolchain:指独立出来的工具链哪种用途的编译，arm(arm-linux-androideabi-4.8),X86(x86-4.8)或MIPS(mipsel-linux-android-4.8)，可cd toolchains中查看并选择适合的类型，我这里使用的是嵌入式；
* --stl:指工具链支持C++ stl，stlport代表C++库将静态链接，stlport_shared将动态链接；
* --install-dir:指安装目录；

注意：因为我使用的是32-bit Ubuntu，独立工具链默认是32位，所以在参数中没有指定系统类型，如果是64-bit Linux系统，需加入--system=linux-x86_64 或MacOSX加入--system=darwin-x86_64。


3、测试程序
```c++
hello.cpp
#include <iostream>
#include <string>
int main(int argc, char **argv)
{
    std::string str = "hello, ndk! this is my own toolchain! ^-^";
    std::cout << str << std::endl;
    return 0;
}
```
Makefile
```makefile
rm=/bin/rm -f
CC=/mnt/my_ndk_toolchain/bin/arm-linux-androideabi-g++
PROGNAME = main
INCLUDES= -I.
CFLAGS  = $(INCLUDES) -g -fPIC -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE
OBJS   = hello.o
LDFLAGS =
all :$(PROGNAME)
%.o: %.cpp
        $(CC) $(CFLAGS) -c -o $@ $<
$(PROGNAME) : $(OBJS)
        @echo  "Linking $(PROGNAME)......"
        ${CC} ${LDFLAGS} -o ${PROGNAME} ${OBJS}
        @echo  "Linking Success!"
clean:
        $(rm) *.o  $(PROGNAME)
```
编译后得到可执行文件:main，adb push到嵌入式Android平台后，./main运行，得到以下结果：
```shell
root@android :/data # ./main                                                
hello, ndk! this is my own toolchain! ^-^
```

# HOWTO Cross compiling on Android
[摘自](https://gist.github.com/Tydus/11109634#file-howto-standalone-toolchain-md)
# 5W1H
## What is NDK
NDK (Native Develop Toolkit) is a toolchain from Android official, originally for users who writes native C/C++ code as JNI library. It's not designed for compiling standalone programs (./a.out) and not compatible with automake/cmake etc.
## What is Standalone Toolchain
"Standalone" refers to two meanings:  
1. The program is standalone (has nothing connect to NDK, and don't need helper scripts to run it)  
2. The toolchain is made for building standalone programs and libs, and which can used by automake etc.
## (Optional) Why NDK is hard to use
By default, NDK uses android flavor directory structure when it's finding headers and libs, which is different from GNU flavor, so the compiler cannot find them. For Example:
```
/home/tyeken8/Desktop/elab/geo/android-ndk-r9d/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/bin/../lib/gcc/arm-linux-androideabi/4.8/../../../../arm-linux-androideabi/bin/ld: error: cannot open crtbegin_dynamic.o: No such file or directory
/home/tyeken8/Desktop/elab/geo/android-ndk-r9d/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/bin/../lib/gcc/arm-linux-androideabi/4.8/../../../../arm-linux-androideabi/bin/ld: error: cannot open crtend_android.o: No such file or directory
/home/tyeken8/Desktop/elab/geo/android-ndk-r9d/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/bin/../lib/gcc/arm-linux-androideabi/4.8/../../../../arm-linux-androideabi/bin/ld: error: cannot find -lc
/home/tyeken8/Desktop/elab/geo/android-ndk-r9d/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/bin/../lib/gcc/arm-linux-androideabi/4.8/../../../../arm-linux-androideabi/bin/ld: error: cannot find -ldl
collect2: error: ld returned 1 exit status
```  
Although we can manuall specify the path (someone wrote a program called "agcc" to handle this automatically, but still not good), it's really annoying.
# Howto
1. Download Android NDK  
    from https://developer.android.com/tools/sdk/ndk/index.html
2. Extract the NDK  
    `tar xf android-ndk-r9d-*.tar.bz2 && cd android-ndk-r9d`
3. Make GNU Android Toolchain from NDK  
    `build/tools/make-standalone-toolchain.sh --toolchain=arm-linux-androideabi-4.8 --platform=android-19 --install-dir=../toolchain`
4. Delete the NDK (Yes, we don't need it any more)  
    `cd .. && rm -rf android-ndk-r9d`
5. Test the native toolchain  
    `cd toolchain/bin`  
    `echo "main(){}" | ./arm-linux-androideabi-gcc -x c - `  
    `file a.out # a.out: ELF 32-bit LSB executable, ARM, version 1 (SYSV), dynamically linked (uses shared libs), not stripped`
6. (Optional) Now you can use it as a standard GNU toolchain  
    For example: `./configure --prefix=/opt/android --host=arm-linux-androideabi && make && make install`


#NDK

##Android NDK 工具链的使用方法（Standalone Toolchain）
[来自](http://blog.csdn.net/smfwuxiao/article/details/6587709)

首先需要确定目标机器的指令集。

如果是 x86 的机器，用 x86-4.4.3 版本的工具链；如果是 arm 指令的，用 arm-linux-androideabi-4.4.3 版本 (x86-4.4.3 和 arm-linux-androideabi-4.4.3 位于ndk目录中)

1、gcc的sysroot 选项

sysroot选项设定 gcc 在编译源码的时候，寻找头文件和库文件的根目录。可以这样调用 gcc --sysroot=/tmp/gcc-arm (及其他选项)。NDK 根目录下的 platforms 目录中的各个子目录的路径都可以直接传给 gcc --sysroot=<dir>。为了简化操作，可以在linux系统的命令终端执行以下命令，设置SYSROOT环境变量，$NDK是ndk的根目录。

$ SYSROOT=$NDK/platforms/android-8/arch-arm

2、调用 NDK gcc（第1种方法）。
 
设置SYSROOT之后，要把它传给 gcc 的--sysroot选项。由于unix/linux自带的gcc并非交叉编译工具，而我们需要使用的是ndk中提供的交叉编译工具（也是gcc），所以需要想办法让编译脚本找到ndk中的gcc，而不要去寻找系统中的gcc。而 unix/linux 系统的编译脚本常常会用 CC 环境变量来引用编译器，所以通过把 CC设置为ndk中的gcc的路径，就能帮助编译脚本找到正确的gcc（我们还能顺便加上--sysroot选项）。

将CC 按如下设置

$ export CC="$NDK/toolchains/<name>/prebuilt/<host-system>/bin/<prefix>gcc --sysroot=$SYSROOT"  
$ $CC -o foo.o -c foo.c  (不必执行这一行，这条命令是调用gcc编译程序）

上面第1行之后之后，再去执行./configure 就可以编译出arm程序了。不过还需要考虑共享库的链接问题，要确保该程序没有链接ndk未提供的共享库。该方法的缺陷就是，不能使用 C++ STL（STLport 或 GNU libstdc++ ），也不能使用异常机制和RTTI。

### 调用NDK编译器（第2种方法，更简单）

1. 创建工具链  
android ndk提供脚本，允许自己定制一套工具链。例如：

$NDK/build/tools/make-standalone-toolchain.sh --platform=android-5 --install-dir=/tmp/my-android-toolchain [ --arch=x86 ]

将会在/tmp/my-android-toolchain 中创建 sysroot 环境和 工具链。--arch 选项选择目标程序的指令架构，默认是为 arm。
如果不加 --install-dir 选项，则会创建 /tmp/ndk/<toolchain-name>.tar.bz2。

2. 设置环境变量  
运行上面make-standalone-toolchain.sh命令创建工具链之后，再：
```shell
$ export PATH=/tmp/my-android-toolchain/bin:$PATH
$ export CC=arm-linux-androideabi-gcc
$ export CXX=arm-linux-androideabi-g++
$ export CXXFLAGS="-lstdc++"
```

3. 使用make  
执行完以上设置环境变量的命令之后，就可以直接编译了（例如，执行 ./configure 然后 make 得到的就是 arm 程序了）。不用再设定 sysroot, CC 了。而且，可以使用 STL，异常，RTTI。

4. make-standalone-toolchain.sh --help 查看帮助  
```shell
$ /cygdrive/f/Android/android-ndk-r10/build/tools/make-standalone-toolchain.sh --help

Usage: make-standalone-toolchain.sh [options]

Generate a customized Android toolchain installation that includes
a working sysroot. The result is something that can more easily be
used as a standalone cross-compiler, e.g. to run configure and
make scripts.

Valid options (defaults are in brackets):

  --help                   Print this help.
  --verbose                Enable verbose mode.
  --toolchain=<name>       Specify toolchain name
  --llvm-version=<ver>     Specify LLVM version
  --stl=<name>             Specify C++ STL [gnustl]
  --arch=<name>            Specify target architecture
  --abis=<list>            Specify list of target ABIs.
  --ndk-dir=<path>         Take source files from NDK at <path> [/cygdrive/f/Android/android-ndk-r10]
  --system=<name>          Specify host system [windows]
  --package-dir=<path>     Place package file in <path> [/tmp/ndk-fangss]
  --install-dir=<path>     Don't create package, install files to <path> instead.
  --platform=<name>        Specify target Android platform/API level. [android-3]
```

* NDK自带工具链

比如，android-ndk-r10自带的工具链，我这里在F:\Android\android-ndk-r10\toolchains目录下：
```shell
fangss@fangss-PC ~
$ ll /cygdrive/f/Android/android-ndk-r10/toolchains
total 0
d---------+ 1 fangss None 0 Dec  9  2014 arm-linux-androideabi-4.6
d---------+ 1 fangss None 0 Dec  9  2014 arm-linux-androideabi-4.8
d---------+ 1 fangss None 0 Dec  9  2014 arm-linux-androideabi-clang3.3
d---------+ 1 fangss None 0 Dec  9  2014 arm-linux-androideabi-clang3.4
d---------+ 1 fangss None 0 Dec  9  2014 llvm-3.3
d---------+ 1 fangss None 0 Dec  9  2014 llvm-3.4
d---------+ 1 fangss None 0 Dec  9  2014 mipsel-linux-android-4.6
d---------+ 1 fangss None 0 Dec  9  2014 mipsel-linux-android-4.8
d---------+ 1 fangss None 0 Dec  9  2014 mipsel-linux-android-clang3.3
d---------+ 1 fangss None 0 Dec  9  2014 mipsel-linux-android-clang3.4
d---------+ 1 fangss None 0 Dec  9  2014 renderscript
d---------+ 1 fangss None 0 Dec  9  2014 x86-4.6
d---------+ 1 fangss None 0 Dec  9  2014 x86-4.8
d---------+ 1 fangss None 0 Dec  9  2014 x86-clang3.3
d---------+ 1 fangss None 0 Dec  9  2014 x86-clang3.4
```
[OUTPUT TRUNCATED]

* 事例

现在我们自己创建，在Cygwin下执行make-standalone-toolchain.sh，如果出现如下权限问题，可以右键管理员身份运行Cygwin。
```shell
fangss@fangss-PC ~
$ /cygdrive/f/Android/android-ndk-r10/build/tools/make-standalone-toolchain.sh --platform=android-9  --install-dir=/cygdrive/f/Android/android-ndk-r10/toolchains/my-arm-linux-androideabi
Auto-config: --toolchain=arm-linux-androideabi-4.6
Copying prebuilt binaries...
find: '/tmp/ndk-fangss/tmp/build-8324/standalone/arm-linux-androideabi-4.6/arm-linux-androideabi': Permission denied
find: '/tmp/ndk-fangss/tmp/build-8324/standalone/arm-linux-androideabi-4.6/bin': Permission denied
find: '/tmp/ndk-fangss/tmp/build-8324/standalone/arm-linux-androideabi-4.6/lib': Permission denied
find: '/tmp/ndk-fangss/tmp/build-8324/standalone/arm-linux-androideabi-4.6/libexec': Permission denied
find: '/tmp/ndk-fangss/tmp/build-8324/standalone/arm-linux-androideabi-4.6/share': Permission denied
mkdir: cannot create directory '/tmp/ndk-fangss/tmp/build-8324/standalone/arm-linux-androideabi-4.6/lib': Permission denied
ERROR: Cannot copy to directory: /tmp/ndk-fangss/tmp/build-8324/standalone/arm-linux-androideabi-4.6/lib/python2.7
```
正确的输出，结果可能my-arm-linux-androideabi文件夹很大
```shell
$ /cygdrive/f/Android/android-ndk-r10/build/tools/make-standalone-toolchain.sh --platform=android-9  --install-dir=/cygdrive/f/Android/android-ndk-r10/toolchains/my-arm-linux-androideabi
Auto-config: --toolchain=arm-linux-androideabi-4.6
Copying prebuilt binaries...
Copying sysroot headers and libraries...
Copying c++ runtime headers and libraries...
Copying files to: /cygdrive/f/Android/android-ndk-r10/toolchains/
Cleaning up...
Done.
```
其中，生成的一些文件：

* add2line：将你要找的地址转成文件和行号，它要使用 debug 信息。
* ar      ：产生、修改和解开一个存档文件
* as      ：gnu的汇编器
* c++filt ：C++ 和 java 中有一种重载函数，所用的重载函数最后会被编译转化成汇编的标，c++filt 就是实现这种反向的转化，根据标号得到函数名。
* gprof   ：gnu 汇编器预编译器。
* ld      ：gnu 的连接器
* nm      ：列出目标文件的符号和对应的地址
* objcopy ：将某种格式的目标文件转化成另外格式的目标文件
* objdump ：显示目标文件的信息
* ranlib  ：为一个存档文件产生一个索引，并将这个索引存入存档文件中
* readelf ：显示 elf 格式的目标文件的信息
* size    ：显示目标文件各个节的大小和目标文件的大小
* strings ：打印出目标文件中可以打印的字符串，有个默认的长度，为4
* strip   ：剥掉目标文件的所有的符号信息


其中strip使用事例如下：

当编译tcpdump时，文件可能比较大，可以使用strip来去掉符号，可减少体积40%。
参考(Monitoring Android Network Traffic Part II: Cross Compiling TCPDUMP)[http://www.symantec.com/connect/blogs/monitoring-android-network-traffic-part-ii-cross-compiling-tcpdump]
(-fPIE -pie编译tcpdump)[http://www.liudonghua.com/?p=372]

```shell
mkdir ~/tcpdump
cd ~/tcpdump
wget http://www.tcpdump.org/release/libpcap-1.6.2.tar.gz
wget http://www.tcpdump.org/release/tcpdump-4.6.2.tar.gz
tar -xzvf libpcap-1.6.2.tar.gz
tar -xzvf tcpdump-4.6.2.tar.gz
mkdir toolchain
%NDK_HOME%/build/tools/make-standalone-toolchain.sh --platform=android-21 --install-dir=~/tcpdump/toolchain
export PATH=~/tcpdump/toolchain/bin:$PATH
export CC=arm-linux-androideabi-gcc
export RANLIB=arm-linux-androideabi-ranlib
export AR=arm-linux-androideabi-ar
export LD=arm-linux-androideabi-ld
cd libpcap-1.6.2
./configure --host=arm-linux --with-pcap=~/tcpdump/libpcap-1.6.2 ac_cv_linux_vers=2
make
cd ../tcpdump-4.6.2
sed -i".bak" "s/setprotoent/\/\/setprotoent/g" print-isakmp.c
sed -i".bak" "s/endprotoent/\/\/endprotoent/g" print-isakmp.c
./configure --host=arm-linux --with-pcap=linux --with-crypto=no ac_cv_linux_vers=2 --disable-ipv6
vi Makefile # 在CFLAGS、LDFLAGS中添加"-fPIE -pie"
make # 或者不用做上面那一步，使用make CFLAGS="-DNBBY=8 -fPIE -pie"
# 最后编译完之后在根目录下即有tcpdump
file tcpdump
tcpdump: ELF 32-bit LSB  shared object, ARM, EABI5 version 1 (SYSV), dynamically linked (uses shared libs), not stripped
```
相应的strip命令
```shell
root@debian $ ls -lh tcpdump
-rwx------ 1 root root 2.8M Aug 18 20:49 tcpdump
root@debian $ arm-linux-gnueabi-strip tcpdump
root@debian $ ls -lh tcpdump
-rwx------ 1 root root 1.5M Aug 18 20:50 tcpdump
```

之后就可以使用了，还可配置如下环境

```shell
export PATH=/cygdrive/f/Android/android-ndk-r10/toolchains/my-arm-linux-androideabi/bin:$PATH
export CC=arm-linux-androideabi-gcc
export RANLIB=arm-linux-androideabi-ranlib
export AR=arm-linux-androideabi-ar
export LD=arm-linux-androideabi-ld
```

再就是可以编译第三方库，如libpcap
```shell
cd libpcap-1.7.4
./configure --host=arm-linux --with-pcap=~/tcpdump/libpcap-1.7.4 ac_cv_linux_vers=2
make
```

## 警告 & 限制

5.1 Windows支持
Windows上的NDK工具链不依赖Cygwin，但是这些工具不能理解Cygwin的路径名（例如，/cygdrive/c/foo/bar）。只能理解C:/cygdrive/c/foo/bar这类路径。不过，NDK 提供的build工具能够很好地应对上述问题（ndk-build）。

5.2 wchar_t 支持  
wchar_t  类型仅从 Android 2.3 开始支持。在 android-9 上， wchar_t 是 4字节。 并且 C语言库提供支持宽字符的函数（例外：multi-byte 编码/解码 函数 和 wsprintf/wsscanf ）在android-9 以前的平台上，wchar_t 是1字节，而且宽字符函数不起作用。建议不使用 wchar_t，提供 wchar_t 支持是为了方便移植以前的代码。

5.3 异常， RTTI 和 STL  
NDK 工具链默认支持C++异常和RTTI（Run Time Type Information），可以用 -fno-exception 和 -fno-rtti 关闭（生成的机器码更小）
注意： 如果要用这两个特性，需要显式链接 libsupc++。例如： arm-linux-androideabi-g++ .... -lsupc++ 
NDK 提供了 libstdc++，因而可以用 STL，但需要显式链接 libstdc++ ( gcc ... -lstdc++)。不过在将来可以不用手动指定这个链接参数。
