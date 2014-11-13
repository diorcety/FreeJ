#ifdef IN_FREEJ
#define FREEJ_INCLUDE(A) "A"
#define FREEJ_SWIG_INCLUDE(A) "A"
#else
#define FREEJ_INCLUDE(A) <freej/A>
#define FREEJ_SWIG_INCLUDE(A) <freej_swig/A>
#endif
