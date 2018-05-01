-- Data package(implementation)

with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

package body Data is


    procedure Output_Scalar(i: in Integer) is
    begin
        -- Put_Line("Output scalar start:");

        Put_Line(Integer'Image(i));

        -- Put_Line("Output scalar end");
    end Output_Scalar;


    procedure Output_Vector(V: access Vector_N) is
    begin
        -- Put_Line("Output vector start:");

        for i in 1..N loop
            Put(Integer'Image(V(i)));
            -- Put(" ");
        end loop;
        Put_Line("");

        -- Put_Line("Output vector end");
    end Output_Vector;


    procedure Output_Matrix(M: access Matrix_N) is
    begin
        -- Put_Line("Output matrix start:");

        for i in 1..N loop
            for j in 1..N loop
                Put(Integer'Image(M(i)(j)));
            end loop;
            Put_Line("");
        end loop;

        -- Put_Line("Output matrix end");
    end Output_Matrix;


    procedure Fill_Vector_Ones(V: access Vector_N)
    is
    begin
        for i in 1..N loop
            V(i) := 1;
        end loop;
    end Fill_Vector_Ones;


    procedure Fill_Matrix_Ones(M: access Matrix_N)
    is
    begin
        for i in 1..N loop
            for j in 1..N loop
                M(i)(j) := 1;
            end loop;
        end loop;
    end Fill_Matrix_Ones;


    function Find_Min(V: Vector_H) return Integer
    is
        m: Integer;
    begin
        m := V(1);
        for i in 1..H loop
            if (V(i) < m) then
                m := V(i);
            end if;
        end loop;

        return m;
    end Find_Min;


    procedure Calc(A: out Vector_H; B: in Vector_H; d: in Integer; x: in Integer; E: in Vector_N; MO: in Matrix_H; MK: in Matrix_N)
    is
        temp, prod: Integer;
    begin
        for t in 1..H loop
            temp := 0;
            for i in 1..N loop
                prod := 0;
                for j in 1..N loop
                    prod := prod + MO(t)(j) * MK(j)(i);
                end loop;

                temp := temp + E(i) * prod;
            end loop;

            A(t) := B(t) * d + x * temp;
        end loop;
    end Calc;


end Data;
