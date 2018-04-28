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


    procedure Input_Vector(V: out Vector) is
    begin
        Put_Line("Input vector start:");

        for i in 1..N loop
            Get(V(i));
        end loop;

        Put_Line("Input vector end");
    end Input_Vector;


    procedure Input_Matrix(M: out Matrix) is
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


    procedure Output_Vector(V: in Vector) is
    begin
        Put_Line("Output vector start:");

        for i in 1..N loop
            Put(Integer'Image(V(i)));
        end loop;
        Put_Line("");

        Put_Line("Output vector end");
    end Output_Vector;


    procedure Output_Matrix(M: in Matrix) is
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


    procedure Fill_Vector_Ones(V: out Vector)
    is
    begin
        for i in 1..N loop
            V(i) := 1;
        end loop;
    end Fill_Vector_Ones;


    procedure Fill_Matrix_Ones(M: out Matrix)
    is
    begin
        for i in 1..N loop
            for j in 1..N loop
                M(i)(j) := 1;
            end loop;
        end loop;
    end Fill_Matrix_Ones;


    function Copy_Vector(V: in Vector) return Vector
    is
        V_Result : Vector;
    begin
        for i in 1..N loop
            V_Result(i) := V(i);
        end loop;

        return V_Result;
    end Copy_Vector;


    function Copy_Matrix(M: in Matrix) return Matrix
    is
        M_Result : Matrix;
    begin
        for i in 1..N loop
            for j in 1..N loop
                M_Result(i)(j) := M(i)(j);
            end loop;
        end loop;

        return M_Result;
    end Copy_Matrix;


    procedure Sort_Vector_H(V: in out Vector; start, finish: in Integer)
    is
        i, j: Integer;
        tmp, pivot: Integer;
    begin
        i := start;
        j := finish;
        pivot := V((start + finish) / 2);

        while (i <= j) loop
            while (V(i) < pivot) loop
                i := i + 1;
            end loop;
            while (V(j) > pivot) loop
                j := j - 1;
            end loop;

            if (i <= j) then
                tmp := V(i);
                V(i) := V(j);
                V(j) := tmp;
                i := i + 1;
                j := j - 1;
            end if;
        end loop;

        if (start < j) then
            Sort_Vector_H(V, start, j);
        end if;
        if (i < finish) then
            Sort_Vector_H(V, i, finish);
        end if;
    end Sort_Vector_H;


    function Sort_Vector_Merge_Halves(V: in Vector) return Vector
    is
        i, j, index: Integer;
        V_Result: Vector;
    begin
        i := 1;
        j := N / 2 + 1;
        index := 1;
        while (index <= N) loop
            if (i >= N / 2 + 1) then
                V_Result(index) := V(j);
                j := j + 1;
            elsif (j >= N + 1) then
                V_Result(index) := V(i);
                i := i + 1;
            elsif (V(i) < V(j)) then
                V_Result(index) := V(i);
                i := i + 1;
            else
                V_Result(index) := V(j);
                j := j + 1;
            end if;

            index := index + 1;
        end loop;

        return V_Result;
    end Sort_Vector_Merge_Halves;


    function Mul_Vector_Matrix_H(
        V: in Vector; M: in Matrix; start, finish: in Integer)
        return Vector
    is
        V_Result : Vector;
    begin
        for i in start..finish loop
            V_Result(i) := 0;
            for j in 1..N loop
                V_Result(i) := V_Result(i) + V(j) * M(j)(i);
            end loop;
        end loop;

        return V_Result;
    end Mul_Vector_Matrix_H;


    function Mul_Matrices_H(
        M1, M2: in Matrix; start, finish: in Integer)
        return Matrix
    is
        M_Result : Matrix;
    begin
        for row in 1..N loop
            for column in start..finish loop
                M_Result(row)(column) := 0;
                for i in 1..N loop
                    M_Result(row)(column) :=
                        M_Result(row)(column) + M1(row)(i) * M2(i)(column);
                end loop;
            end loop;
        end loop;

        return M_Result;
    end Mul_Matrices_H;


    procedure Add_Vectors_H(
        V_Result: out Vector; V1, V2: in Vector; start, finish: in Integer)
    is
    begin
        for i in start..finish loop
            V_Result(i) := V1(i) + V2(i);
        end loop;
    end Add_Vectors_H;


end Data;
