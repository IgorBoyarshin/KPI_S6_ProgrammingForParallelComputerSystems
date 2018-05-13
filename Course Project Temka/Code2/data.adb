-- Data package(implementation)

with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

package body Data is


    procedure Output_Matrix(M: access Matrix_N) is
    begin
        for i in 1..N loop
            for j in 1..N loop
                Put(Integer'Image(M(i)(j)));
            end loop;
            Put_Line("");
        end loop;
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


    procedure Calc(MA: out Matrix_H; MB: in Matrix_H; MC: in Matrix_N; x: in Integer; MK: in Matrix_H)
    is
        temp: Integer;
    begin
        for t in 1..H loop
            for i in 1..N loop
                temp := 0;
                for j in 1..N loop
                    temp := temp + MB(t)(j) * MC(j)(i);
                end loop;

                MA(t)(i) := temp + x * MK(t)(i);
            end loop;
        end loop;
    end Calc;


end Data;
