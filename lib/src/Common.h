/***********************************************\
*                   Common.h                    *
*           George Koskeridis (C) 2016          *
\***********************************************/
//LIBRARY INTERNAL HEADER. DO NOT INCLUDE THIS SEPARATELY.

#ifndef __EduDS_COMMON_H
#define __EduDS_COMMON_H

#ifdef _WIN32
# ifdef _MSC_VER //disable compiler bug on MSVC where it warns about
#  pragma warning(disable : 4127) //do{}while(0) macros
# endif
# ifdef EXPORT_API_EduDS_DLL
#  define EduDS_API __declspec(dllexport)
# elif IMPORT_API_EduDS_DLL
#  define EduDS_API __declspec(dllimport)
# else
#  define EduDS_API
# endif
# define EduDS_LOCAL
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EduDS_API    __attribute__ ((visibility ("default")))
#   define EduDS_LOCAL  __attribute__ ((visibility ("hidden")))
#  else
#   define EduDS_API
#   define EduDS_LOCAL
#  endif
# else
#  define EduDS_API
#  define EduDS_LOCAL
# endif
#endif

#define SAVE_ERR(err, tmp_err) \
    if (err) \
        *(err) = tmp_err;

typedef void (*UserDataCallback)(void *pData);
typedef int (*UserCompareCallback)(const void *pKey1, const void *pKey2);
typedef size_t (*UserHashFuncCallback)(size_t key_hash, size_t array_len);

typedef enum _TreeTraversalMethod {
    PRE_ORDER,
    IN_ORDER,
    POST_ORDER,
    BREADTH_FIRST, //level-order
    EULER
} TreeTraversalMethod;

//Key-value pair type definition
typedef struct _KeyValuePair {
    void *pData;
    void *pKey;
} KeyValuePair;

//EduDS library error codes
typedef enum _EduDSErrCode {
    EduDS_SUCCESS,
    EduDS_MALLOC_FAIL,
    EduDS_KEY_EXISTS,
    EduDS_INVALID_ARGS
} EduDSErrCode;

//ErrorHandling.c functions are declared here instead of using another header file
EduDS_API const char *EduDSErrString(EduDSErrCode err);

#endif //__EduDS_COMMON_H
