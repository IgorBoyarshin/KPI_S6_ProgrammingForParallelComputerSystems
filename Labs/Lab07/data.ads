-- Data package(specification)

generic
    N: Integer;
    H: Integer;
package Data is

    type Vector is array(Integer range<>) of Integer;
    subtype Vector_H is Vector(1..1*H);
    subtype Vector_2H is Vector(1..2*H);
    subtype Vector_4H is Vector(1..4*H);
    subtype Vector_N is Vector(1..N);

    type Matrix is array(Integer range<>) of Vector_N;
    subtype Matrix_H is Matrix(1..1*H);
    subtype Matrix_2H is Matrix(1..2*H);
    subtype Matrix_4H is Matrix(1..4*H);
    subtype Matrix_N is Matrix(1..N);

    procedure Output_Scalar(i: in Integer);
    procedure Output_Vector(V: access Vector_N);
    procedure Output_Matrix(M: access Matrix_N);

    procedure Fill_Vector_Ones(V: access Vector_N);
    procedure Fill_Matrix_Ones(M: access Matrix_N);

    function Find_Min(V: Vector_H) return Integer;

    procedure Calc(A: out Vector_H; B: in Vector_H; d: in Integer; x: in Integer; E: in Vector_N; MO: in Matrix_H; MK: in Matrix_N);

end Data;
