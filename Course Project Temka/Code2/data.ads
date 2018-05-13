-- Data package(specification)

generic
    N: Integer;
    H: Integer;
package Data is

    type Vector is array(Integer range<>) of Integer;
    subtype Vector_H is Vector(1..1*H);
    subtype Vector_2H is Vector(1..2*H);
    subtype Vector_3H is Vector(1..3*H);
    subtype Vector_N is Vector(1..N);

    type Matrix is array(Integer range<>) of Vector_N;
    subtype Matrix_H is Matrix(1..1*H);
    subtype Matrix_2H is Matrix(1..2*H);
    subtype Matrix_3H is Matrix(1..3*H);
    subtype Matrix_N is Matrix(1..N);

    procedure Output_Matrix(M: access Matrix_N);

    procedure Fill_Vector_Ones(V: access Vector_N);
    procedure Fill_Matrix_Ones(M: access Matrix_N);

    function Find_Min(V: Vector_H) return Integer;

    procedure Calc(MA: out Matrix_H; MB: in Matrix_H; MC: in Matrix_N; x: in Integer; MK: in Matrix_H);

end Data;
