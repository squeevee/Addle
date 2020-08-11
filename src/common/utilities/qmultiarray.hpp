#ifndef QMULTIARRAY_HPP
#define QMULTIARRAY_HPP

#include <type_traits>
#include <boost/multi_array.hpp>
#include <boost/array.hpp>

#include <QSharedData>
#include <QList>
#include <QSize>
#include <QPoint>
#include <QRect>
namespace Addle {

/**
 * @class QMultiArray
 * @brief Implicitly shared wrapper for Boost.MultiArray
 * 
 * Some extra sugar is given for 2D arrays.
 */
template<typename T, std::size_t N, typename Allocator = std::allocator<T>>
class QMultiArray
{
    struct QMultiArrayData : QSharedData
    {
        QMultiArrayData() = default;

        explicit QMultiArrayData(const QList<typename boost::multi_array<T, N, Allocator>::size_type>& sizes,
            const typename boost::multi_array<T, N, Allocator>::storage_order_type& store,
            const Allocator& alloc)
            : arr(sizes, store, alloc)
        {
        }

        explicit QMultiArrayData(const boost::detail::multi_array::extent_gen<N>& ranges,
            const typename boost::multi_array<T, N, Allocator>::storage_order_type& store,
            const Allocator& alloc)
            : arr(ranges, store, alloc)
        {
        }

        QMultiArrayData(const boost::multi_array<T,N>& x)
            : arr(x)
        {
        }

        QMultiArrayData(const QMultiArrayData&) = default;

        boost::multi_array<T, N, Allocator> arr;
    };
public:
    typedef typename boost::multi_array<T, N, Allocator>::element                element;
    typedef typename boost::multi_array<T, N, Allocator>::value_type             value_type;
    typedef typename boost::multi_array<T, N, Allocator>::reference              reference;
    typedef typename boost::multi_array<T, N, Allocator>::const_reference        const_reference;
    typedef typename boost::multi_array<T, N, Allocator>::difference_type        difference_type;
    typedef typename boost::multi_array<T, N, Allocator>::iterator               iterator;
    typedef typename boost::multi_array<T, N, Allocator>::const_iterator         const_iterator;
    typedef typename boost::multi_array<T, N, Allocator>::reverse_iterator       reverse_iterator;
    typedef typename boost::multi_array<T, N, Allocator>::const_reverse_iterator const_reverse_iterator;
    typedef typename boost::multi_array<T, N, Allocator>::size_type              size_type;
    typedef typename boost::multi_array<T, N, Allocator>::index                  index;
    typedef typename boost::multi_array<T, N, Allocator>::index_gen              index_gen;
    typedef typename boost::multi_array<T, N, Allocator>::index_range            index_range;
    typedef typename boost::multi_array<T, N, Allocator>::extent_gen             extent_gen;
    typedef typename boost::multi_array<T, N, Allocator>::extent_range           extent_range;
    typedef typename boost::multi_array<T, N, Allocator>::storage_order_type     storage_order_type;

    static const std::size_t dimensionality = N;

    QMultiArray() : _data(new QMultiArrayData) {}
    QMultiArray(const QMultiArray&) = default;
    QMultiArray(QMultiArray&& other)
        : _data(other._data)
    {
        other._data = nullptr;
    }

    explicit QMultiArray(const QList<size_type>& sizes,
        const storage_order_type& store = boost::c_storage_order(),
        const Allocator& alloc = Allocator())
        : _data(new QMultiArrayData(sizes, store, alloc))
    {
    }

    template<typename = typename std::enable_if<N == 2>::type>
    explicit QMultiArray(QSize size,
        const storage_order_type& store = boost::c_storage_order(),
        const Allocator& alloc = Allocator())
        : _data(new QMultiArrayData(boost::extents[size.width()][size.height()], store, alloc))
    {
    }

    QMultiArray(const boost::multi_array<T, N, Allocator>& x)
    {
    }

    QMultiArray& operator=(const QMultiArray&) = default;

    inline const boost::multi_array<T, N, Allocator>& constArray() const { return _data->arr; }
    inline const boost::multi_array<T, N, Allocator>& array() const { return _data->arr; }
    inline boost::multi_array<T, N, Allocator>& array() { return _data->arr; }

    inline QList<size_type> shape() const
    {
        QList<size_type> shape;
        shape.reserve(N);
        for (int i = 0; i < N; i++)
        {
            shape.append(_data->arr.shape()[i]);
        }
        return shape;
    }

    template<typename = typename std::enable_if<N == 2>::type>
    inline QSize size() const
    {
        return QSize(_data->arr.shape()[0], _data->arr.shape()[1]);
    }

    template<typename = typename std::enable_if<N == 2>::type>
    inline QRect rect() const
    {
        return QRect(QPoint(), size());
    }

    template<typename = typename std::enable_if<N == 2>::type>
    inline size_type width() const
    {
        return _data->arr.shape()[0];
    }

    template<typename = typename std::enable_if<N == 2>::type>
    inline size_type height() const
    {
        return _data->arr.shape()[1];
    }

    inline size_type num_elements() const
    {
        return _data->arr.num_elements();
    }



    inline iterator begin() { return _data->arr.begin(); }
    inline const_iterator begin() const { return _data->arr.begin(); }

    inline iterator end() { return _data->arr.end(); }
    inline const_iterator end() const { return _data->arr.end(); }

    inline reverse_iterator rbegin() { return _data->arr.rbegin(); }
    inline const_reverse_iterator rbegin() const { return _data->arr.rbegin(); }

    inline reverse_iterator rend() { return _data->arr.rend(); }
    inline const_reverse_iterator rend() const { return _data->arr.rend(); }


    template <typename ExtentList>
    inline void resize(const ExtentList& extents) { _data->arr.resize(extents); }

    inline typename std::enable_if<N == 2, void>::type
    resize(QSize size) { _data->arr.resize(boost::extents[size.width()][size.height()]); }

    inline typename std::enable_if<N == 2, void>::type
    resize(size_type width, size_type height) { _data->arr.resize(boost::extents[width][height]); }


    template <typename IndexList>
    inline element& rAt(const IndexList& indices) { return _data->arr(indices); }

    inline typename std::enable_if<N == 2, element&>::type
    rAt(QPoint pos) { return _data->arr[pos.x()][pos.y()]; }

    inline typename std::enable_if<N == 2, element&>::type
    rAt(index x, index y) { return _data->arr[x][y]; }

    template <typename IndexList>
    void insert(const IndexList& indices, const element& value) { _data->arr(indices) = value; }

    inline typename std::enable_if<N == 2, void>::type
    insert(QPoint pos, const element& value) { _data->arr[pos.x()][pos.y()] = value; }

    inline typename std::enable_if<N == 2, void>::type
    insert(index x, index y, const element& value) { _data->arr[x][y] = value; }

    inline const element& at(const QList<index>& indices) const { return _data->arr(indices); }
    
    inline typename std::enable_if<N == 2, const element&>::type
    at(QPoint pos) const { return _data->arr[pos.x()][pos.y()]; }

    inline typename std::enable_if<N == 2, const element&>::type
    at(index x, index y) const { return _data->arr[x][y]; }

    inline reference operator[](index i) { return _data->arr[i]; }
    inline const_reference operator[](index i) const { return _data->arr[i]; }

    inline bool boundContains(const QList<index>& indices) const
    {
        for (auto i : indices)
        {
            if (i < 0) return false;
            if (i >= _data->arr.shape()[i]) return false;
        }
        return true;
    }

    inline typename std::enable_if<N == 2, bool>::type
    boundContains(QPoint pos) const
    {
        return pos.x() >= 0 && pos.x() < width()
            && pos.y() >= 0 && pos.y() < height();
    }

    inline typename std::enable_if<N == 2, bool>::type
    boundContains(int x, int y) const
    {
        return x >= 0 && x < width()
            && y >= 0 && y < height();
    }

private:
    QSharedDataPointer<QMultiArrayData> _data;
};

template<typename T, class Allocator = std::allocator<T>>
class ConstRowViewer
{
public:
    class iterator
    {
        using index_range = boost::multi_array_types::index_range;

    public:
        typedef typename boost::multi_array_ref<T, 2>::template const_array_view<1>::type row_type;

        iterator() = default;
        iterator(const iterator&) = default;

        inline bool operator==(const iterator& x) const { return _arr_impl == x._arr_impl && _index == x._index; }
        inline bool operator!=(const iterator& x) const { return !(*this == x); }

        inline row_type operator*() const { return (*_arr_impl)[boost::indices[index_range()][_index]]; }

        inline iterator& operator++() { ++_index; return *this; }

    private:
        iterator(const boost::multi_array<T, 2, Allocator>& arr_impl, int index)
            : _arr_impl(&arr_impl), _index(index)
        {
        }
        
        const boost::multi_array<T, 2, Allocator>* _arr_impl = nullptr;
        int _index = 0;

        friend class ConstRowViewer;
    };

    ConstRowViewer(const QMultiArray<T, 2, Allocator>& arr)
        : _arr(arr)
    {
    }

    iterator begin() const
    {
        return iterator(_arr.array(), 0);
    }

    iterator end() const
    {
        return iterator(_arr.array(), _arr.height());
    }

private:
    const QMultiArray<T, 2, Allocator> _arr;
};

template<typename T, class Allocator = std::allocator<T>>
class MutableRowViewer
{
public:
    class iterator
    {
        using index_range = boost::multi_array_types::index_range;

    public:
        typedef typename boost::multi_array_ref<T, 2>::template array_view<1>::type row_type;

        iterator() = default;
        iterator(const iterator&) = default;

        inline bool operator==(const iterator& x) const { return _arr_impl == x._arr_impl && _index == x._index; }
        inline bool operator!=(const iterator& x) const { return !(*this == x); }

        inline row_type operator*() const { return (*_arr_impl)[boost::indices[index_range()][_index]]; }

        inline iterator& operator++() { ++_index; return *this; }

    private:
        iterator(boost::multi_array<T, 2, Allocator>& arr_impl, int index)
            : _arr_impl(&arr_impl), _index(index)
        {
        }
        
        boost::multi_array<T, 2, Allocator>* _arr_impl = nullptr;
        int _index = 0;

        friend class MutableRowViewer;
    };

    MutableRowViewer(QMultiArray<T, 2, Allocator>& arr)
        : _arr(arr)
    {
    }

    iterator begin() const
    {
        return iterator(_arr.array(), 0);
    }

    iterator end() const
    {
        return iterator(_arr.array(), _arr.height());
    }

private:
    QMultiArray<T, 2, Allocator>& _arr;
};

} // namespace Addle
#endif // QMULTIARRAY_HPP