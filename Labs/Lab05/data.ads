-- Data package(specification)

generic
    N: Integer;
package Data is

    type Vector is array (1..N) of Integer;
    type Matrix is array (1..N) of Vector;
    -- type Vector is private;
    -- type Matrix is private;


    procedure Input_Scalar (i: out Integer);
    procedure Input_Vector (V: access Vector);
    procedure Input_Matrix (M: access Matrix);
    procedure Output_Scalar(i: in Integer);
    procedure Output_Vector(V: access Vector);
    procedure Output_Matrix(M: access Matrix);


    procedure Fill_Vector_Ones(V: access Vector);
    procedure Fill_Matrix_Ones(M: access Matrix);

    function Copy_Vector(V: access Vector) return access Vector;
    function Copy_Matrix(M: access Matrix) return access Matrix;


    -- function Mul_Vector_Matrix_H(
    --     V: in Vector; M: in Matrix; start, finish: in Integer)
    --     return Vector;
    -- function Mul_Matrices_H(
    --     M1, M2: in Matrix; start, finish: in Integer)
    --     return Matrix;
    -- procedure Add_Vectors_H(
    --     V_Result: out Vector; V1, V2: in Vector; start, finish: in Integer);


    -- private
    -- type Vector is array (1..N) of Integer;
    -- type Matrix is array (1..N) of Vector;

end Data;
