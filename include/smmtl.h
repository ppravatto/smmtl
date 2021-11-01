#ifndef SMMTL_H
#define SMMTL_H

#include <exception>

namespace smmtl{

    namespace _except_{

        class BoundError: public std::exception {
            virtual const char* what() const throw(){
                return "Index out of bounds";
            }
        };

        class InitError: public std::exception {
            virtual const char* what() const throw(){
                return "Operation performed on uninitilized object";
            }
        };

        class ShapeError: public std::exception {
            virtual const char* what() const throw(){
                return "Mismatch between the objects shape";
            }
        };

    }

    template <typename T>
    class BasicArray{
        protected:
            bool init;
            int nrows, ncols;
            T* data;
            void copy(const BasicArray<T>& source_);
            void check_limits(int row_, int col_);
            void check_init();
            void verify_elementwise_operation(const BasicArray<T>& obj_);

        public:
            BasicArray();
            BasicArray(int nrows_, int ncols_);
            BasicArray(int nrows_, int ncols_, T* pdata_);
            BasicArray(const BasicArray<T>& source_);
            virtual ~BasicArray();
            BasicArray<T>& operator= (const BasicArray<T>& source_);
            BasicArray<T>& operator+ (const BasicArray<T>& source_);
            BasicArray<T>& operator- (const BasicArray<T>& source_);
            BasicArray<T>& operator+= (const BasicArray<T>& source_);
            BasicArray<T>& operator-= (const BasicArray<T>& source_);
    };
    
    template <typename T>
    void BasicArray<T>::copy(const BasicArray<T>& source_){
        init = source_.init;
        nrows = source_.nrows;
        ncols = source_.ncols;
        if(init == true){
            data = new T [nrows*ncols];
            for(int i=0; i<nrows*ncols; i++) data[i] = source_.data[i];
        }
    }

    template <typename T>
    void BasicArray<T>::check_limits(int row_, int col_){
        if(row_<0 || row_>=nrows || col_<0 || col_>=ncols) throw _except_::BoundError();
    }

    template <typename T>
    void BasicArray<T>::check_init(){
        if(init==false) throw _except_::InitError();
    }

    template <typename T>
    void verify_elementwise_operation(const BasicArray<T>& obj_){
        check_init(); obj_.check_init();
        if(nrows != obj_.nrows || ncols != obj_.ncols) throw _except_::ShapeError();
    }

    template <typename T>
    BasicArray<T>::BasicArray() : init(false) {}

    template <typename T>
    BasicArray<T>::BasicArray(int nrows_, int ncols_) : init(true), nrows(nrows_), ncols(ncols_){
        data = new T [nrows*ncols];
        for(int i=0; i<nrows_*ncols_; i++) data[i] = T(0);
    }

    template <typename T>
    BasicArray<T>::BasicArray(int nrows_, int ncols_, T* pdata_) : init(true), nrows(nrows_), ncols(ncols_){
        data = new T [nrows*ncols];
        for(int i=0; i<nrows_*ncols_; i++) data[i] = pdata_[i];
    }
    
    template <typename T>
    BasicArray<T>::BasicArray(const BasicArray<T>& source_){
        copy(source_);
    }

    template <typename T>
    BasicArray<T>::~BasicArray(){
        if(init == false) delete[] data;
    }
    
    template <typename T>
    BasicArray<T>& BasicArray<T>::operator= (const BasicArray<T>& source_){
        if(init == true) delete[] data;
        copy(source_);
        return *this;
    }

    template <typename T>
    BasicArray<T>& BasicArray<T>::operator+ (const BasicArray<T>& source_){
        verify_elementwise_operation(source_);
        T* buffer = new T [ncols*nrows];
        for(int i=0; i<nrows*ncols; i++) buffer[i] = data[i] + source_.data[i];
        BasicArray<T> Result(nrows, ncols, buffer);
        delete[] buffer;
        return *this;
    }

    template <typename T>
    BasicArray<T>& BasicArray<T>::operator- (const BasicArray<T>& source_){
        verify_elementwise_operation(source_);
        T* buffer = new T [ncols*nrows];
        for(int i=0; i<nrows*ncols; i++) buffer[i] = data[i] - source_.data[i];
        BasicArray<T> Result(nrows, ncols, buffer);
        delete[] buffer;
        return *this;
    }

    template <typename T>
    BasicArray<T>& BasicArray<T>::operator+= (const BasicArray<T>& source_){
        verify_elementwise_operation(source_);
        for(int i=0; i<nrows*ncols; i++) data[i] += source_.data[i];
        return *this;
    }

    template <typename T>
    BasicArray<T>& BasicArray<T>::operator-= (const BasicArray<T>& source_){
        verify_elementwise_operation(source_);
        for(int i=0; i<nrows*ncols; i++) data[i] -= source_.data[i];
        return *this;
    }

}

#endif