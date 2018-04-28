-------------------------------------------------------------------------------
-- Lab1: Ada. Semaphores.
-- Task: A = sort(B + C) + Z * (MT * MK)
-- Author: Igor Boyarshin
-- Date: 20.02.2018
-------------------------------------------------------------------------------
with Ada.Text_IO, Ada.Integer_Text_IO, Ada.Synchronous_Task_Control, System.Multiprocessors, Data;
use Ada.Text_IO, Ada.Integer_Text_IO, Ada.Synchronous_Task_Control, System.Multiprocessors;

procedure Lab1
is
    N: constant := 2000;
    H: constant := N / 2;
    Memory: constant := 50000000;
    Output_Threshold: constant := 7;

    package My_Data is new Data(N);
    use My_Data;

    A, B, C, Z, T : Vector;
    MK, MT : Matrix;
    S0, S1, S2, S3, S4 : Suspension_Object;

    procedure Tasks_Start is
        -- Tasks specification
        task T1 is
            pragma Task_Name("Task 1");
            pragma Storage_Size(Memory);
        end T1;

        task T2 is
            pragma Task_Name("Task 2");
            pragma Storage_Size(Memory);
        end T2;


        -- Tasks implementation
        task body T1
        is
            Z1: Vector;
            MT1: Matrix;
        begin
            Put_Line(":> Starting Task 1...");

            -- Input
            Fill_Vector_Ones(B);
            Fill_Vector_Ones(C);
            Fill_Vector_Ones(Z);
            Fill_Matrix_Ones(MT);
            Fill_Matrix_Ones(MK);

            -- Signal T2 about input end
            Set_True(S0);

            -- Copy Z, MT
            Suspend_Until_True(S4);
                Z1 := Copy_Vector(Z);
                MT1 := Copy_Matrix(MT);
            Set_True(S4);

            -- Calculations 1
            Add_Vectors_H(T, B, C, 1, H);
            Sort_Vector_H(T, 1, H);

            -- Wait for calculations 1 end in T2
            Suspend_Until_True(S1);

            -- Calculations 2
            T := Sort_Vector_Merge_Halves(T);

            -- Signal T2 about calculations 2 end
            Set_True(S2);

            -- Calculations 3
            Add_Vectors_H(
                A,
                T,
                Mul_Vector_Matrix_H(
                    Z1,
                    Mul_Matrices_H(
                        MT1,
                        MK,
                        1, H
                    ),
                    1, H
                ),
                1, H
            );

            -- Wait for calculations 3 end in T2
            Suspend_Until_True(S3);

            -- Output
            if (N <= Output_Threshold) then
                Output_Vector(A);
            end if;

            Put_Line(":> Finished Task 1");
        end T1;


        task body T2
        is
            Z2: Vector;
            MT2: Matrix;
        begin
            Put_Line(":> Starting Task 2...");

            -- Wait for input end in T1
            Suspend_Until_True(S0);

            -- Calculations 1
            Add_Vectors_H(T, B, C, H + 1, N);
            Sort_Vector_H(T, H + 1, N);

            -- Signal T1 about calculations 1 end
            Set_True(S1);

            -- Copy Z, MT
            Suspend_Until_True(S4);
                Z2 := Copy_Vector(Z);
                MT2 := Copy_Matrix(MT);
            Set_True(S4);

            -- Wait for calculations 2 end in T1
            Suspend_Until_True(S2);

            -- Calculations 3
            Add_Vectors_H(
                A,
                T,
                Mul_Vector_Matrix_H(
                    Z2,
                    Mul_Matrices_H(
                        MT2,
                        MK,
                        H + 1, N
                    ),
                    H + 1, N
                ),
                H + 1, N
            );

            -- Signal T1 about calculations 3 end
            Set_True(S3);

            Put_Line(":> Finished Task 2");
        end T2;

    begin
        null;
    end Tasks_Start;


begin
    Set_True(S4);
    Tasks_Start;
end Lab1;
