#ifndef CUSTOM_H
#define CUSTOM_H

#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_ALWAYS_NATIVE

#if defined(LOG4QT_EXPORTS)
#   undef LOG4QT_EXPORTS
#   define LOG4QT_EXPORTS __declspec(dllexport)
#elif defined(LOG4QT_IMPORTS)
#   undef LOG4QT_IMPORTS
#   define LOG4QT_EXPORTS __declspec(dllimport)
#else
#   define LOG4QT_EXPORTS
#endif

#endif // CUSTOM_H
