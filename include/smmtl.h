#ifndef SMMTL_H
#define SMMTL_H

#include <iostream>
#include <string>
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
    class Table{
        protected:
            bool init;
            int nrows, ncols;
            T* data;
            void copy(const Table<T>& source_);
            void check_limits(int row_, int col_) const;
            void check_init() const;
            void verify_elementwise_operation(const Table<T>& obj_) const;

        public:
            Table();
            Table(int nrows_, int ncols_);
            Table(int nrows_, int ncols_, T* pdata_);
            Table(const Table<T>& source_);
            virtual ~Table();
            Table<T>& operator= (const Table<T>& source_);
            Table<T> operator+ (const Table<T>& source_);
            Table<T> operator- (const Table<T>& source_);
            Table<T>& operator+= (const Table<T>& source_);
            Table<T>& operator-= (const Table<T>& source_);

            template <typename Variable>
                friend std::ostream& operator<<(std::ostream &os, const Table<Variable> &source);

            template < typename Variable, typename Scalar, typename std::enable_if<!(std::is_base_of<Table<Variable>, Scalar>::value), Scalar>::type*>
                friend Table<Variable> operator*(const Table<Variable> &Array, Scalar scalar);
            
            template < typename Variable, typename Scalar, typename std::enable_if<!(std::is_base_of<Table<Variable>, Scalar>::value), Scalar>::type*>
                friend Table<Variable> operator*(Scalar scalar, const Table<Variable> &Array);

    };
    
    template <typename T>
    void Table<T>::copy(const Table<T>& source_){
        init = source_.init;
        nrows = source_.nrows;
        ncols = source_.ncols;
        if(init == true){
            data = new T [nrows*ncols];
            for(int i=0; i<nrows*ncols; i++) data[i] = source_.data[i];
        }
    }

    template <typename T>
    void Table<T>::check_limits(int row_, int col_) const {
        if(row_<0 || row_>=nrows || col_<0 || col_>=ncols) throw _except_::BoundError();
    }

    template <typename T>
    void Table<T>::check_init() const {
        if(init==false) throw _except_::InitError();
    }

    template <typename T>
    void Table<T>::verify_elementwise_operation(const Table<T>& obj_) const {
        check_init(); obj_.check_init();
        if(nrows != obj_.nrows || ncols != obj_.ncols) throw _except_::ShapeError();
    }

    template <typename T>
    Table<T>::Table() : init(false) {}

    template <typename T>
    Table<T>::Table(int nrows_, int ncols_) : init(true), nrows(nrows_), ncols(ncols_){
        data = new T [nrows*ncols];
        for(int i=0; i<nrows_*ncols_; i++) data[i] = T(0);
    }

    template <typename T>
    Table<T>::Table(int nrows_, int ncols_, T* pdata_) : init(true), nrows(nrows_), ncols(ncols_){
        data = new T [nrows*ncols];
        for(int i=0; i<nrows_*ncols_; i++) data[i] = pdata_[i];
    }
    
    template <typename T>
    Table<T>::Table(const Table<T>& source_){
        copy(source_);
    }

    template <typename T>
    Table<T>::~Table(){
        if(init == false) delete[] data;
    }
    
    template <typename T>
    Table<T>& Table<T>::operator= (const Table<T>& source_){
        if(init == true) delete[] data;
        copy(source_);
        return *this;
    }

    template <typename T>
    Table<T> Table<T>::operator+ (const Table<T>& source_){
        verify_elementwise_operation(source_);
        T* buffer = new T [ncols*nrows];
        for(int i=0; i<nrows*ncols; i++) buffer[i] = data[i] + source_.data[i];
        Table<T> Result(nrows, ncols, buffer);
        delete[] buffer;
        return Result;
    }

    template <typename T>
    Table<T> Table<T>::operator- (const Table<T>& source_){
        verify_elementwise_operation(source_);
        T* buffer = new T [ncols*nrows];
        for(int i=0; i<nrows*ncols; i++) buffer[i] = data[i] - source_.data[i];
        Table<T> Result(nrows, ncols, buffer);
        delete[] buffer;
        return Result;
    }

    template <typename T>
    Table<T>& Table<T>::operator+= (const Table<T>& source_){
        verify_elementwise_operation(source_);
        for(int i=0; i<nrows*ncols; i++) data[i] += source_.data[i];
        return *this;
    }

    template <typename T>
    Table<T>& Table<T>::operator-= (const Table<T>& source_){
        verify_elementwise_operation(source_);
        for(int i=0; i<nrows*ncols; i++) data[i] -= source_.data[i];
        return *this;
    }

    //Define operator << overload to print the Table content
    template <typename Variable>
    std::ostream& operator<<(std::ostream &os, const Table<Variable> &source) {
        if(source.init == false){
            os << "uninitalized object at address <" << &source << ">" << std::endl;
        }
        else{
            for(int r=0; r<source.nrows; r++){
                for(int c=0; c<source.nrows; c++){
                    os << source.data[c + r*source.ncols] << '\t';
                }
                os << std::endl;
            }
        }
        return os;
    }


    //Define operators to compute the product between a scalar and an array type object

    template < typename Variable,
               typename Scalar,
               typename std::enable_if<!(std::is_base_of<Table<Variable>, Scalar>::value), Scalar>::type* = nullptr >

    Table<Variable> operator*(const Table<Variable> &Array, Scalar scalar){
        Array.check_init();
        Table<Variable> Result(Array.nrows, Array.ncols);
        for(int i=0; i<Result.nrows*Result.ncols; i++) Result.data[i] = Variable(scalar)*Array.data[i];
        return Result;
    }

    template < typename Variable,
               typename Scalar,
               typename std::enable_if<!(std::is_base_of<Table<Variable>, Scalar>::value), Scalar>::type* = nullptr >

    Table<Variable> operator*(Scalar scalar, const Table<Variable> &Array){
        return Array*scalar;
    }

}

#endif