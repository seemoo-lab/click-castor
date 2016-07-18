/* include/click/config-android.h. */
#ifndef CLICK_CONFIG_ANDROID_H
#define CLICK_CONFIG_ANDROID_H

/* Define if you have the <execinfo.h> header file. */
#undef HAVE_EXECINFO_H

/* Define if you have the <ifaddrs.h> header file. */
#undef HAVE_IFADDRS_H

/* Define if you have -lpcap and pcap.h. */
#undef HAVE_PCAP

/* Define if the C++ compiler understands static_assert. */
#undef HAVE_CXX_STATIC_ASSERT

/* Define that we are building for Android */
#define HAVE_ANDROID 1

/* Define macros that surround Click declarations. */
#ifndef CLICK_DECLS
# define CLICK_DECLS		/* */
# define CLICK_ENDDECLS		/* */
# define CLICK_USING_DECLS	/* */
# define CLICK_NAME(name)	::name
#endif

#endif /* CLICK_CONFIG_ANDROID_H */
