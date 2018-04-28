-- Data package(specification)

generic
    N: Integer;
package Data is

    -- type Vector is array (1..N) of Integer;
    -- type Matrix is array (1..N) of Vector;
    type Vector is private;
    type Matrix is private;


    procedure Input_Scalar (i: out Integer);
    procedure Input_Vector (V: out Vector);
    procedure Input_Matrix (M: out Matrix);
    procedure Output_Scalar(i: in Integer);
    procedure Output_Vector(V: in Vector);
    procedure Output_Matrix(M: in Matrix);


    procedure Fill_Vector_Ones(V: out Vector);
    procedure Fill_Matrix_Ones(M: out Matrix);

    function Copy_Vector(V: in Vector) return Vector;
    function Copy_Matrix(M: in Matrix) return Matrix;


    procedure Sort_Vector_H(V: in out Vector; start, finish: in Integer);
    function Sort_Vector_Merge_Halves(V: in Vector) return Vector;
    function Mul_Vector_Matrix_H(
        V: in Vector; M: in Matrix; start, finish: in Integer)
        return Vector;
    function Mul_Matrices_H(
        M1, M2: in Matrix; start, finish: in Integer)
        return Matrix;
    procedure Add_Vectors_H(
        V_Result: out Vector; V1, V2: in Vector; start, finish: in Integer);


    private
    type Vector is array (1..N) of Integer;
    type Matrix is array (1..N) of Vector;

end Data;
