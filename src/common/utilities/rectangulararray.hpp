#ifndef RECTANGULARARRAY_HPP
#define RECTANGULARARRAY_HPP

#include <QVector>
#include <QSize>
#include <QPoint>
#include <QSharedData>
#include <QSharedDataPointer>
#include "interfaces/traits/compat.hpp"

template<typename T>
class RectangularArray
{
    struct RectangularArrayData : QSharedData
    {
        RectangularArrayData() = default;
        RectangularArrayData(QSize size)
            : size(size)
        {
            contents.fill(QVector<T>(size.width()), size.height());
        }

        QVector<QVector<T>> contents;
        // `contents` is organized into rows then columns
        // i.e., the data in the cell at (4,2) is in (row = 2, column = 4)
        // and thus accessible as contents[2][4]
        //
        // NOTE: rows are indexed with y-coordinates and columns are indexed
        // with x-coordinates. 

        QSize size;
    };

public:
    RectangularArray() : _data(new RectangularArrayData) { }
    RectangularArray(const RectangularArray& other) : _data(other._data) { }
    RectangularArray(QSize size) : _data(new RectangularArrayData(size)) { }

    inline bool boundaryContains(QPoint position) const
    {
        return position.x() >= 0
            && position.y() >= 0
            && position.x() < _data->size.width()
            && position.y() < _data->size.height();
    }

    inline void resize(QSize size)
    {
        _data->contents.resize(size.height());
        for (QVector<T>& row : _data->contents)
        {
            row.resize(size.width());
        }
    }

    inline void insert(QPoint position, const T& value)
    {
        _data->contents[position.y()][position.x()] = value;
    }

    inline const T& at(QPoint position) const
    {
        return _data->contents[position.y()][position.x()];
    }

    inline T& rAt(QPoint position)
    {
        return _data->contents[position.y()][position.x()];
    }

    //TODO: probably want some rectangular operations
        // Insert one rectangle into another
        // Copy out a rectangular subsection
            // Copy out rows / columns
        // Rearrange rows and columns
        // Resize up and to the left
        // Translation / reflection / discrete rotation

private:
    QSharedDataPointer<RectangularArrayData> _data;
};

#endif // RECTANGULARARRAY_HPP