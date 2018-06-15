#ifndef UTILS_H
#define UTILS_H

#include <qcstring.h>

#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;
#endif


/**
 * Helper class to init python interpriter on constrution and finalize it
 * on destruction.
 */
class PyInitHelper
{
public:
    PyInitHelper();
    ~PyInitHelper();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/**
 * Simple smart pointer over PyObject.
 * It decrements its reference on destruction (if not marked as borrowed).
 */
class PyObjectPtr
{
public:
    PyObjectPtr(PyObject *p = NULL);
    ~PyObjectPtr();

    /**
     * Release python object reference.
     * This method decrements reference count if required and resets pointer.
     */
    void release()
    {
        decRef();
        reset();
    }

    /** Return object reference. */
    PyObject* get() const { return m_p; }

    /**
     * Take object reference.
     *
     * This method returns the reference and resets pointer.
     */
    PyObject* take()
    {
        PyObject *r = m_p;
        reset();
        return r;
    }

    operator PyObject*() const { return m_p; }
    PyObject* operator->() const { return m_p; }

    operator bool() const { return !isNull(); }
    bool isNull() const { return m_p ? false : true; }

    PyObject* operator=(PyObject *other)
    {
        decRef();
        reset(other);
        return other;
    }

    /**
     * Set or reset borrowed reference state.
     * <em>Borrowed</em> means - ownership is not transferred,
     * and we don't need to decrement reference.
     * @param state Borrowed state.
     */
    void setBorrowed(bool state = true)
    {
        m_needDecRef = !state;
    }

    /**
     * Increment wrapped object reference.
     *
     * Note what borrowed flag will be reset.
     */
    void incRef();


    /**
     * Decrement wrapped object reference.
     * But only if borrowed flag is not set.
     */
    void decRef();

private:
    PyObject *m_p;
    bool m_needDecRef;

    const PyObjectPtr& operator=(const PyObjectPtr &other);

    void reset(PyObject *other = NULL);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/**
 * Base class to create python class wrappers.
 */
class PyClass
{
public:
    PyClass(PyObject *object = NULL);
    virtual ~PyClass();

    PyObjectPtr& ptr() { return m_object; }
    const PyObjectPtr& ptr() const { return m_object; }

    PyObject* get() const { return m_object.get(); }
    PyObject* take() const { return m_object.get(); }
    void release() { m_object.release(); }

    bool isNull() const { return m_object.isNull(); }

    bool ensureCreated();

protected:
    bool m_createError;
    PyObjectPtr m_object;

    virtual PyObject* create() = 0;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/**
 * Python tuple wrapper.
 */
class PyTuple: public PyClass
{
public:
    PyTuple(int size);

    void add(const QCString &str);
    void add(PyObject *object);

protected:
    PyObject* create() override;

private:
    int m_size;
    int m_index;

    // Prevent add ptr to tuple since the tupe steals a reference.
    void add(const PyObjectPtr &ptr);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


class PyDict: public PyClass
{
public:
    PyDict(PyObject *object = NULL);

    void setField(const QCString &field, const QCString &value);
    void setField(const QCString &field, PyObject *object);
    void setField(const QCString &field, const PyObjectPtr &ptr);

protected:
    PyObject* create() override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//class MultiString
//{
//public:
//    MultiString(int count);

//    void add(const QCString &value);

//    bool isMultiValue() const
//    {
//        return !m_tuple.ptr().isNull();
//    }

//    const QCString& str() const { return m_str; }
//    PyObject* pyTuple() const { return m_tuple.ptr().get(); }

//private:
//    QCString m_str;
//    PyTuple m_tuple;
//};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#endif // UTILS_H
