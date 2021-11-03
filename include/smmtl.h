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

    
    //Predeclaration of the main structures

    template <typename T> class Table;
    template <typename T> class Mat;
    template <typename T> class Vec;

    //Definition of the Table template class
    
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
            void verify_matrix_product(const Table<T>& obj_) const;

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

            void transpose();
            Table<T> t();

            template <typename Variable>
                friend std::ostream& operator<<(std::ostream &os, const Table<Variable> &source);

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
    void Table<T>::verify_matrix_product(const Table<T>& obj_) const {
        this->check_init(); obj_.check_init();
        if(this->ncols != obj_.nrows) throw _except_::ShapeError();
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

    template <typename T>
    void Table<T>::transpose(){
        check_init();
        T* buffer = new T [nrows*ncols];
        for(int i=0; i<nrows*ncols; i++) buffer[i] = data[i];
        for(int r=0; r<nrows; r++){
            for(int c=0; c<ncols; c++){
                data[r + nrows*c] = buffer[c + ncols*r];
            }
        }
        int tmp = nrows;
        nrows = ncols; ncols = tmp;
    }

    template <typename T>
    Table<T> Table<T>::t(){
        Table<T> copy = *this;
        copy.transpose();
        return copy;
    }


    //Define operator << overload to print the Table content

    template <typename Variable>
    std::ostream& operator<<(std::ostream &os, const Table<Variable> &source) {
        if(source.init == false){
            os << "uninitalized object at address <" << &source << ">" << std::endl;
        }
        else{
            for(int r=0; r<source.nrows; r++){
                for(int c=0; c<source.ncols; c++){
                    os << source.data[c + r*source.ncols] << '\t';
                }
                os << std::endl;
            }
        }
        return os;
    }
    
    //Define the Mat template class

    template <typename T>
    class Mat : public Table<T>{
        public:
            Mat();
            Mat(int nrows_, int ncols_);
            Mat(int nrows_, int ncols_, T* pdata_);
            Mat(const Mat<T>& source_);
            explicit Mat(const Table<T>& source_);
            ~Mat(){};

            Mat<T>& operator= (const Mat<T>& source_);
            Mat<T> operator+ (const Mat<T>& source_);
            Mat<T> operator- (const Mat<T>& source_);
            Mat<T>& operator+= (const Mat<T>& source_);
            Mat<T>& operator-= (const Mat<T>& source_);
            T& operator() (int row_, int col_);
            
            Mat<T> t();
            bool is_square();

            template <typename Variable>
                friend Mat<Variable> operator* (const Mat<Variable>& A, const Mat<Variable>& B);
            
            template <typename Variable>
                friend Vec<Variable> operator* (const Mat<Variable>& A, const Vec<Variable>& v);
            
            template <typename Variable>
                friend Vec<Variable> operator* (const Vec<Variable>& v, const Mat<Variable>& A);
            
            template < typename Variable, typename Scalar, typename std::enable_if<!(std::is_base_of<Table<Variable>, Scalar>::value), Scalar>::type*>
                friend Mat<Variable> operator*(const Mat<Variable> &Matrix, Scalar scalar);
            
            template < typename Variable, typename Scalar, typename std::enable_if<!(std::is_base_of<Table<Variable>, Scalar>::value), Scalar>::type*>
                friend Mat<Variable> operator*(Scalar scalar, const Mat<Variable> &Matrix);
    };

    template <typename T>
    Mat<T>::Mat() : Table<T>() {}

    template <typename T>
    Mat<T>::Mat(int nrows_, int ncols_) : Table<T>(nrows_, ncols_) {}

    template <typename T>
    Mat<T>::Mat(int nrows_, int ncols_, T* pdata_) : Table<T>(nrows_, ncols_, pdata_) {}

    template <typename T>
    Mat<T>::Mat(const Mat<T>& source_) : Table<T>(source_) {}

    template <typename T>
    Mat<T>::Mat(const Table<T>& source_) : Table<T>(source_) {}

    template <typename T>
    Mat<T>& Mat<T>::operator= (const Mat<T>& source_){
        Table<T>::operator=(source_);
        return *this;
    }

    template <typename T>
    Mat<T> Mat<T>::operator+ (const Mat<T>& source_){
        Mat<T> Result(Table<T>::operator+(source_));
        return Result;
    }

    template <typename T>
    Mat<T> Mat<T>::operator- (const Mat<T>& source_){
        Mat<T> Result(Table<T>::operator-(source_));
        return Result;
    }

    template <typename T>
    Mat<T>& Mat<T>::operator+= (const Mat<T>& source_){
        Table<T>::operator+=(source_);
        return *this;
    }

    template <typename T>
    Mat<T>& Mat<T>::operator-= (const Mat<T>& source_){
        Table<T>::operator-=(source_);
        return *this;
    }

    template <typename T>
    T& Mat<T>::operator() (int row_, int col_){
        return this->data[col_ + row_*this->ncols];
    }

    template <typename T>
    Mat<T> Mat<T>::t(){
        Mat<T> Result(Table<T>::t());
        return Result;
    }

    template <typename T>
    bool Mat<T>::is_square(){
        this->check_init();
        return (this->nrows == this->ncols)? true : false;
    }


    template <typename T>
    class Vec : public Table<T>{
        public:
            Vec();
            Vec(int nelements_);
            Vec(int nelements_, T* pdata_);
            Vec(const Vec<T>& source_);
            explicit Vec(const Table<T>& source_);
            ~Vec(){};

            Vec<T>& operator= (const Vec<T>& source_);
            Vec<T> operator+ (const Vec<T>& source_);
            Vec<T> operator- (const Vec<T>& source_);
            Vec<T>& operator+= (const Vec<T>& source_);
            Vec<T>& operator-= (const Vec<T>& source_);
            T& operator() (int index_);

            Vec<T> t();

            template <typename Variable>
                friend Variable operator* (const Vec<Variable>& A, const Vec<Variable>& B);
            
            template <typename Variable>
                friend Vec<Variable> operator* (const Mat<Variable>& A, const Vec<Variable>& v);
            
            template <typename Variable>
                friend Vec<Variable> operator* (const Vec<Variable>& v, const Mat<Variable>& A);

            template < typename Variable, typename Scalar, typename std::enable_if<!(std::is_base_of<Table<Variable>, Scalar>::value), Scalar>::type*>
                friend Vec<Variable> operator*(const Vec<Variable> &Vector, Scalar scalar);
            
            template < typename Variable, typename Scalar, typename std::enable_if<!(std::is_base_of<Table<Variable>, Scalar>::value), Scalar>::type*>
                friend Vec<Variable> operator*(Scalar scalar, const Vec<Variable> &Vector);
    };

    template <typename T>
    Vec<T>::Vec() : Table<T>() {}

    template <typename T>
    Vec<T>::Vec(int nelements_) : Table<T>(nelements_, 1) {}

    template <typename T>
    Vec<T>::Vec(int nelements_, T* pdata_) : Table<T>(nelements_, 1, pdata_) {}

    template <typename T>
    Vec<T>::Vec(const Vec<T>& source_) : Table<T>(source_) {}

    template <typename T>
    Vec<T>::Vec(const Table<T>& source_) : Table<T>(source_) {
        if(this->ncols != 1. && this->nrows != 1.) throw _except_::ShapeError();
    }

    template <typename T>
    Vec<T>& Vec<T>::operator= (const Vec<T>& source_){
        Table<T>::operator=(source_);
        return *this;
    }

    template <typename T>
    Vec<T> Vec<T>::operator+ (const Vec<T>& source_){
        Vec<T> Result(Table<T>::operator+(source_));
        return Result;
    }

    template <typename T>
    Vec<T> Vec<T>::operator- (const Vec<T>& source_){
        Vec<T> Result(Table<T>::operator-(source_));
        return Result;
    }

    template <typename T>
    Vec<T>& Vec<T>::operator+= (const Vec<T>& source_){
        Table<T>::operator+=(source_);
        return *this;
    }

    template <typename T>
    Vec<T>& Vec<T>::operator-= (const Vec<T>& source_){
        Table<T>::operator-=(source_);
        return *this;
    }

    template <typename T>
    T& Vec<T>::operator() (int element_){
        return this->data[element_];
    }

    template <typename T>
    Vec<T> Vec<T>::t(){
        Vec<T> Result(Table<T>::t());
        return Result;
    }
    

    // Define matrix product operations between Mat and Vec classes

    template <typename Variable>
    Mat<Variable> operator* (const Mat<Variable>& A, const Mat<Variable>& B){
        A.verify_matrix_product(B);
        Mat<Variable> Result(A.nrows, B.ncols);
        for(int r=0; r<A.nrows; r++){
            for(int c=0; c<B.ncols; c++){
                Variable element = Variable(0.);
                for(int i=0; i<A.ncols; i++){
                    element += A.data[i + r*A.ncols] * B.data[c + i*B.ncols];
                }
                Result(r, c) = element;
            }
        }
        return Result;
    }

    template <typename Variable>
    Vec<Variable> operator* (const Mat<Variable>& A, const Vec<Variable>& v){
        A.verify_matrix_product(v);
        Vec<Variable> Result(A.nrows);
        for(int r=0; r<A.nrows; r++){
            Variable element = Variable(0.);
            for(int i=0; i<v.nrows; i++){
                element += A.data[i + r*A.ncols]*v.data[i];
            }
            Result(r) = element;
        }
        return Result;
    }
            
    template <typename Variable>
    Vec<Variable> operator* (const Vec<Variable>& v, const Mat<Variable>& A){
        v.verify_matrix_product(A);
        Vec<Variable> Result(A.ncols);
        Result.transpose();
        for(int c=0; c<A.ncols; c++){
            Variable element = Variable(0.);
            for(int i=0; i<v.ncols; i++){
                element += A.data[c + i*A.ncols]*v.data[i];
            }
            Result(c) = element;
        }
        return Result;
    }

    template <typename Variable>
    Variable operator* (const Vec<Variable>& A, const Vec<Variable>& B){
        A.verify_matrix_product(B);
        Variable result = Variable(0.);
        for(int i=0; i<B.nrows; i++) result += A.data[i]*B.data[i];
        return result;
    }


    // Define product with scalar of Mat and Vec classes
    
    template < typename Variable,
               typename Scalar,
               typename std::enable_if<!(std::is_base_of<Table<Variable>, Scalar>::value), Scalar>::type* = nullptr >

    Mat<Variable> operator*(const Mat<Variable> &Matrix, Scalar scalar){
        Matrix.check_init();
        Mat<Variable> Result(Matrix.nrows, Matrix.ncols);
        for(int i=0; i<Result.nrows*Result.ncols; i++) Result.data[i] = Variable(scalar)*Matrix.data[i];
        return Result;
    }

    template < typename Variable,
               typename Scalar,
               typename std::enable_if<!(std::is_base_of<Table<Variable>, Scalar>::value), Scalar>::type* = nullptr >

    Mat<Variable> operator*(Scalar scalar, const Mat<Variable> &Matrix){
        return Matrix*scalar;
    }

    template < typename Variable,
               typename Scalar,
               typename std::enable_if<!(std::is_base_of<Table<Variable>, Scalar>::value), Scalar>::type* >
    
    Vec<Variable> operator*(const Vec<Variable> &Vector, Scalar scalar){
        Vector.check_init();
        Mat<Variable> Result(Vector.nrows, Vector.ncols);
        for(int i=0; i<Result.nrows*Result.ncols; i++) Result.data[i] = Variable(scalar)*Vector.data[i];
        return Result;
    }
            
    template < typename Variable,
               typename Scalar,
               typename std::enable_if<!(std::is_base_of<Table<Variable>, Scalar>::value), Scalar>::type* >
    
    Vec<Variable> operator*(Scalar scalar, const Vec<Variable> &Vector){
        return Vector*scalar;
    }

}

#endif