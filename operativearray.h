#ifndef OPERATIVEARRAY_H
#define OPERATIVEARRAY_H

#pragma once

template<typename dType> class OperativeArray
{
private:
    dType* data;
    int count;
    dType* diff = nullptr;

public:
    OperativeArray<dType>(dType* source, int count)
    {
        this->data = source;
        this->count = count;
        this->diff = new dType[count];
    }

    int getCount() { return this->count; }
    dType* getData() { return this->data; }

    inline bool operator==(OperativeArray& val)
    {
        if(this->count != val.getCount()) { return false; }

        for (int i = 0; i < count; ++i)
        {
            if (*(data + static_cast<dType>(i)) != *(val.getData() + static_cast<dType>(i))) { return false; }
        }
        return true;
    }

    inline bool operator!=(OperativeArray& val)
    {
        if (this->count != val.getCount()) { return false; }
        bool flag = false;

        for (int i = 0; i < count; ++i)
        {
            if (*(data + static_cast<dType>(i)) != *(val.getData() + static_cast<dType>(i))) { flag = true; }
        }
        return flag ? flag : false;
    }

    inline bool operator<(OperativeArray& val)
    {
        if (this->count != val.getCount()) { return false; }
        bool flag = false;

        for (int i = 0; i < count; ++i)
        {
            if ( (*(data + static_cast<dType>(i)) > *(val.getData() + static_cast<dType>(i))) && flag == false) { return false; }
            else if (*(data + static_cast<dType>(i)) < *(val.getData() + static_cast<dType>(i))) { flag = true; }
        }
        return flag ? flag : false;
    }

    inline bool operator>(OperativeArray& val)
    {
        if (this->count != val.getCount()) { return false; }
        bool flag = false;

        for (int i = 0; i < count; ++i)
        {
            if ((*(data + static_cast<dType>(i)) < * (val.getData() + static_cast<dType>(i))) && flag == false) { return false; }
            else if (*(data + static_cast<dType>(i)) > *(val.getData() + static_cast<dType>(i))) { flag = true; }
        }
        return flag ? flag : false;
    }

    inline bool operator<=(OperativeArray& val)
    {
        if ((*this == val) || (*this < val))
        {
            return true;
        }
    }

    inline bool operator>=(OperativeArray& val)
    {
        if ((*this == val) || (*this > val))
        {
            return true;
        }
    }

    inline dType* operator+(OperativeArray& val)
    {
        for(int i = 0; i < count; ++i)
        {
            *(diff+i) = *(data+i) + *(val.getData() + 1);
        }
        return diff;
    }

    inline dType* operator-(OperativeArray& val)
    {
        for(int i = 0; i < count; ++i)
        {
            *(diff+i) = *(data+i) - *(val.getData() + 1);
        }
        return diff;
    }

    inline dType* operator*(OperativeArray& val)
    {
        for(int i = 0; i < count; ++i)
        {
            *(diff+i) = *(data+i) * *(val.getData() + 1);
        }
        return diff;
    }

    inline dType* operator/(OperativeArray& val)
    {
        for(int i = 0; i < count; ++i)
        {
            *(diff+i) = *(data+i) / *(val.getData() + 1);
        }
        return diff;
    }

    inline dType* operator%(OperativeArray& val)
    {
        for(int i = 0; i < count; ++i)
        {
            *(diff+i) = *(data+i) % *(val.getData() + 1);
        }
        return diff;
    }

    inline dType* operator=(OperativeArray& val)
    {
        for(int i = 0; i < val.getCount(); ++i)
        {
            *(data+i) = *(val.getData()+i);
        }
    }
};



#endif // OPERATIVEARRAY_H
