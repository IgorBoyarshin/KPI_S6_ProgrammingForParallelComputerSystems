-- Data package(implementation)

with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

package body Data is


    procedure Input_Scalar(i: out Integer) is
    begin
        Put_Line("Input scalar start:");

        Get(i);

        Put_Line("Input scalar end");
    end Input_Scalar;


    procedure Input_Vector(V: access Vector) is
    begin
        Put_Line("Input vector start:");

        for i in 1..N loop
            Get(V(i));
        end loop;

        Put_Line("Input vector end");
    end Input_Vector;


    procedure Input_Matrix(M: access Matrix) is
    begin
        Put_Line("Input matrix start:");

        for i in 1..N loop
            for j in 1..N loop
                Get(M(i)(j));
            end loop;
        end loop;

        Put_Line("Input matrix end");
    end Input_Matrix;


    procedure Output_Scalar(i: in Integer) is
    begin
        Put_Line("Output scalar start:");

        Put_Line(Integer'Image(i));

        Put_Line("Output scalar end");
    end Output_Scalar;


    procedure Output_Vector(V: access Vector) is
    begin
        Put_Line("Output vector start:");

        for i in 1..N loop
            Put(Integer'Image(V(i)));
            Put(" ");
        end loop;
        Put_Line("");

        Put_Line("Output vector end");
    end Output_Vector;


    procedure Output_Matrix(M: access Matrix) is
    begin
        Put_Line("Output matrix start:");

        for i in 1..N loop
            for j in 1..N loop
                Put(Integer'Image(M(i)(j)));
            end loop;
            Put_Line("");
        end loop;

        Put_Line("Output matrix end");
    end Output_Matrix;


    procedure Fill_Vector_Ones(V: access Vector)
    is
    begin
        for i in 1..N loop
            V(i) := 1;
        end loop;
    end Fill_Vector_Ones;


    procedure Fill_Matrix_Ones(M: access Matrix)
    is
    begin
        for i in 1..N loop
            for j in 1..N loop
                M(i)(j) := 1;
            end loop;
        end loop;
    end Fill_Matrix_Ones;


    function Copy_Vector(V: access Vector) return access Vector
    is
        V_Result : access Vector := new Vector;
    begin
        for i in 1..N loop
            V_Result(i) := V(i);
        end loop;

        return V_Result;
    end Copy_Vector;


    function Copy_Matrix(M: access Matrix) return access Matrix
    is
        M_Result : access Matrix := new Matrix;
    begin
        for i in 1..N loop
            for j in 1..N loop
                M_Result(i)(j) := M(i)(j);
            end loop;
        end loop;

        return M_Result;
    end Copy_Matrix;


end Data;
