-------------------------------------------------------------------------------
-- Lab7: Ada. Randezvous
-- Task: A = B*d + min(Z) * E * (MO*MK)
-- Author: Igor Boyarshin
-- Date: 01.05.2018
-------------------------------------------------------------------------------
with Ada.Text_IO, Ada.Integer_Text_IO, Ada.Synchronous_Task_Control, System.Multiprocessors, Data;
use Ada.Text_IO, Ada.Integer_Text_IO, Ada.Synchronous_Task_Control, System.Multiprocessors;

procedure Main
is
    N: constant := 16;
    P: constant := 8;
    H: constant := N / P;
    Memory: constant := 5000000;
    Output_Threshold: constant := 16;

    package My_Data is new Data(N, H);
    use My_Data;
-------------------------------------------------------------------------------
    procedure Tasks_Start is
        -- Tasks specification
        task T1 is
            pragma Task_Name("Task 1");
            pragma Storage_Size(Memory);

            entry Set3(d_in: in Integer; MO_in: in Matrix_2H);
            entry Set5(Z_in: in Vector_H; E_in: in Vector_N);
            entry SetX2(x_other: in Integer);
            entry SetX3(x_other: in Integer);
            entry SetX5(x_other: in Integer);
            entry SetA2(A_in: in Vector_H);
            entry SetA3(A_in: in Vector_2H);
            entry SetA5(A_in: in Vector_4H);
        end T1;
        task T2 is
            pragma Task_Name("Task 2");
            pragma Storage_Size(Memory);

            entry Set1(B_in: in Vector_4H; MK_in: in Matrix_N);
            entry Set4(d_in: in Integer; MO_in: in Matrix_2H);
            entry Set6(Z_in: in Vector_H; E_in: in Vector_N);
            entry SetX1(x_new: in Integer);
        end T2;
        task T3 is
            pragma Task_Name("Task 3");
            pragma Storage_Size(Memory);

            entry Set4(d_in: in Integer; MO_in: in Matrix_4H);
            entry Set1(B_in: in Vector_2H; MK_in: in Matrix_N);
            entry Set7(Z_in: in Vector_H; E_in: in Vector_N);
            entry SetX4(x_other: in Integer);
            entry SetX1(x_new: in Integer);
            entry SetA4(A_in: in Vector_H);
        end T3;
        task T4 is
            pragma Task_Name("Task 4");
            pragma Storage_Size(Memory);

            entry Set2(B_in: in Vector_2H; MK_in: in Matrix_N);
            entry Set8(Z_in: in Vector_H; E_in: in Vector_N);
            entry SetX3(x_new: in Integer);
        end T4;
        task T5 is
            pragma Task_Name("Task 5");
            pragma Storage_Size(Memory);

            entry Set7(Z_in: in Vector_2H; E_in: in Vector_N);
            entry Set1(B_in: in Vector_H; MK_in: in Matrix_N; d_in: Integer; MO_in: in Matrix_H);
            entry SetX6(x_other: in Integer);
            entry SetX7(x_other: in Integer);
            entry SetX1(x_new: in Integer);
            entry SetA6(A_in: in Vector_H);
            entry SetA7(A_in: in Vector_2H);
        end T5;
        task T6 is
            pragma Task_Name("Task 6");
            pragma Storage_Size(Memory);

            entry Set8(Z_in: in Vector_2H; E_in: in Vector_N);
            entry Set2(B_in: in Vector_H; MK_in: in Matrix_N; d_in: Integer; MO_in: in Matrix_H);
            entry SetX5(x_new: in Integer);
        end T6;
        task T7 is
            pragma Task_Name("Task 7");
            pragma Storage_Size(Memory);

            entry Set8(Z_in: in Vector_4H; E_in: in Vector_N);
            entry Set3(B_in: in Vector_H; MK_in: in Matrix_N; d_in: Integer; MO_in: in Matrix_H);
            entry SetX8(x_other: in Integer);
            entry SetX5(x_new: in Integer);
            entry SetA8(A_in: in Vector_H);
        end T7;
        task T8 is
            pragma Task_Name("Task 8");
            pragma Storage_Size(Memory);

            entry Set4(B_in: in Vector_H; MK_in: in Matrix_N; d_in: Integer; MO_in: in Matrix_H);
            entry SetX7(x_new: in Integer);
        end T8;
        -- Tasks implementation
-------------------------------------------------------------------------------
        task body T1
        is
            A, B: access Vector_N := new Vector_N;
            MK: access Matrix_N := new Matrix_N;

            d, x: Integer;
            MO: access Matrix_2H := new Matrix_2H;
            Z: access Vector_H := new Vector_H;
            E: access Vector_N := new Vector_N;
        begin
            Put_Line(":> Starting Task 1...");

            Fill_Vector_Ones(B);
            Fill_Matrix_Ones(MK);

            T2.Set1(B.all(4*H+1..8*H), MK.all);

            T3.Set1(B.all(2*H+1..4*H), MK.all);

            accept Set3(d_in: in Integer; MO_in: in Matrix_2H) do
                d:= d_in;
                MO.all := MO_in;
            end Set3;

            T5.Set1(B.all(H+1..2*H), MK.all, d, MO.all(H+1..2*H));

            accept Set5(Z_in: in Vector_H; E_in: in Vector_N) do
                Z.all := Z_in;
                E.all := E_in;
            end Set5;

            x := Find_Min(Z.all);

            accept SetX2(x_other: in Integer) do
                if (x_other < x) then
                    x := x_other;
                end if;
            end SetX2;
            accept SetX3(x_other: in Integer) do
                if (x_other < x) then
                    x := x_other;
                end if;
            end SetX3;
            accept SetX5(x_other: in Integer) do
                if (x_other < x) then
                    x := x_other;
                end if;
            end SetX5;

            T5.SetX1(x);
            T3.SetX1(x);
            T2.SetX1(x);

            Calc(A.all(1..H), B.all(1..H), d, x, E.all, MO.all(1..H), MK.all);

            accept SetA2(A_in: in Vector_H) do
                A.all(H+1..2*H) := A_in;
            end SetA2;
            accept SetA3(A_in: in Vector_2H) do
                A.all(2*H+1..4*H) := A_in;
            end SetA3;
            accept SetA5(A_in: in Vector_4H) do
                A.all(4*H+1..8*H) := A_in;
            end SetA5;

            if (N <= Output_Threshold) then
                Output_Vector(A);
            end if;

            Put_Line(":> Finished Task 1");
        end T1;
-------------------------------------------------------------------------------
        task body T2
        is
            A: access Vector_H := new Vector_H;
            B: access Vector_4H := new Vector_4H;
            d: Integer;
            Z: access Vector_H := new Vector_H;
            E: access Vector_N := new Vector_N;
            MO: access Matrix_2H := new Matrix_2H;
            MK: access Matrix_N := new Matrix_N;
            x: Integer;
        begin
            Put_Line(":> Starting Task 2...");

            accept Set1(B_in: in Vector_4H; MK_in: in Matrix_N) do
                B.all := B_in;
                MK.all := MK_in;
            end Set1;

            accept Set4(d_in: in Integer; MO_in: in Matrix_2H) do
                d := d_in;
                MO.all := MO_in;
            end Set4;

            T4.Set2(B.all(2*H+1..4*H), MK.all);

            accept Set6(Z_in: in Vector_H; E_in: in Vector_N) do
                Z.all := Z_in;
                E.all := E_in;
            end Set6;

            T6.Set2(B.all(H+1..2*H), MK.all, d, MO.all(H+1..2*H));

            x := Find_Min(Z.all);

            T1.SetX2(x);

            accept SetX1(x_new: in Integer) do
                x := x_new;
            end SetX1;

            Calc(A.all(1..H), B.all(1..H), d, x, E.all, MO.all(1..H), MK.all);

            T1.SetA2(A.all);

            Put_Line(":> Finished Task 2");
        end T2;
-------------------------------------------------------------------------------
        task body T3
        is
            A: access Vector_2H := new Vector_2H;
            B: access Vector_2H := new Vector_2H;
            d: Integer;
            Z: access Vector_H := new Vector_H;
            E: access Vector_N := new Vector_N;
            MO: access Matrix_4H := new Matrix_4H;
            MK: access Matrix_N := new Matrix_N;
            x: Integer;
        begin
            Put_Line(":> Starting Task 3...");

            accept Set4(d_in: in Integer; MO_in: in Matrix_4H) do
                d := d_in;
                MO.all := MO_in;
            end Set4;

            accept Set1(B_in: in Vector_2H; MK_in: in Matrix_N) do
                B.all := B_in;
                MK.all := MK_in;
            end Set1;

            T1.Set3(d, MO.all(2*H+1..4*H));

            accept Set7(Z_in: in Vector_H; E_in: in Vector_N) do
                Z.all := Z_in;
                E.all := E_in;
            end Set7;

            T7.Set3(B.all(H+1..2*H), MK.all, d, MO.all(H+1..2*H));

            x := Find_Min(Z.all);

            accept SetX4(x_other: in Integer) do
                if (x_other < x) then
                    x := x_other;
                end if;
            end SetX4;

            T1.SetX3(x);

            accept SetX1(x_new: in Integer) do
                x := x_new;
            end SetX1;

            T4.SetX3(x);

            Calc(A.all(1..H), B.all(1..H), d, x, E.all, MO.all(1..H), MK.all);

            accept SetA4(A_in: in Vector_H) do
                A.all(H+1..2*H) := A_in;
            end SetA4;

            T1.SetA3(A.all);

            Put_Line(":> Finished Task 3");
        end T3;
-------------------------------------------------------------------------------
        task body T4
        is
            A: access Vector_H := new Vector_H;
            B: access Vector_2H := new Vector_2H;
            d: Integer;
            Z: access Vector_H := new Vector_H;
            E: access Vector_N := new Vector_N;
            MO: access Matrix_N := new Matrix_N;
            MK: access Matrix_N := new Matrix_N;
            x: Integer;
        begin
            Put_Line(":> Starting Task 4...");

            d := 1;
            Fill_Matrix_Ones(MO);

            T3.Set4(d, MO.all(4*H+1..8*H));
            T2.Set4(d, MO.all(2*H+1..4*H));

            accept Set2(B_in: in Vector_2H; MK_in: in Matrix_N) do
                B.all := B_in;
                MK.all := MK_in;
            end Set2;

            accept Set8(Z_in: in Vector_H; E_in: in Vector_N) do
                Z.all := Z_in;
                E.all := E_in;
            end Set8;

            T8.Set4(B.all(H+1..2*H), MK.all, d, MO.all(H+1..2*H));

            x := Find_Min(Z.all);

            T3.SetX4(x);

            accept SetX3(x_new: in Integer) do
                x := x_new;
            end SetX3;

            Calc(A.all(1..H), B.all(1..H), d, x, E.all, MO.all(1..H), MK.all);

            T3.SetA4(A.all);

            Put_Line(":> Finished Task 4");
        end T4;
-------------------------------------------------------------------------------
        task body T5
        is
            A: access Vector_4H := new Vector_4H;
            B: access Vector_H := new Vector_H;
            d: Integer;
            Z: access Vector_2H := new Vector_2H;
            E: access Vector_N := new Vector_N;
            MO: access Matrix_H := new Matrix_H;
            MK: access Matrix_N := new Matrix_N;
            x: Integer;
        begin
            Put_Line(":> Starting Task 5...");

            accept Set7(Z_in: in Vector_2H; E_in: in Vector_N) do
                Z.all := Z_in;
                E.all := E_in;
            end Set7;

            accept Set1(B_in: in Vector_H; MK_in: in Matrix_N; d_in: Integer; MO_in: in Matrix_H) do
                B.all := B_in;
                MK.all := MK_in;
                d := d_in;
                MO.all := MO_in;
            end Set1;

            T1.Set5(Z.all(H+1..2*H), E.all);

            x := Find_Min(Z.all(1..H));

            accept SetX6(x_other: in Integer) do
                if (x_other < x) then
                    x := x_other;
                end if;
            end SetX6;

            accept SetX7(x_other: in Integer) do
                if (x_other < x) then
                    x := x_other;
                end if;
            end SetX7;

            T1.SetX5(x);

            accept SetX1(x_new: in Integer) do
                x := x_new;
            end SetX1;

            T7.SetX5(x);
            T6.SetX5(x);

            Calc(A.all(1..H), B.all(1..H), d, x, E.all, MO.all(1..H), MK.all);

            accept SetA6(A_in: in Vector_H) do
                A.all(H+1..2*H) := A_in;
            end SetA6;
            accept SetA7(A_in: in Vector_2H) do
                A.all(2*H+1..4*H) := A_in;
            end SetA7;

            T1.SetA5(A.all);

            Put_Line(":> Finished Task 5");
        end T5;
-------------------------------------------------------------------------------
        task body T6
        is
            A: access Vector_H := new Vector_H;
            B: access Vector_H := new Vector_H;
            d: Integer;
            Z: access Vector_2H := new Vector_2H;
            E: access Vector_N := new Vector_N;
            MO: access Matrix_H := new Matrix_H;
            MK: access Matrix_N := new Matrix_N;
            x: Integer;
        begin
            Put_Line(":> Starting Task 6...");

            accept Set8(Z_in: in Vector_2H; E_in: in Vector_N) do
                Z.all := Z_in;
                E.all := E_in;
            end Set8;

            T2.Set6(Z.all(H+1..2*H), E.all);

            accept Set2(B_in: in Vector_H; MK_in: in Matrix_N; d_in: Integer; MO_in: in Matrix_H) do
                B.all := B_in;
                MK.all := MK_in;
                d := d_in;
                MO.all := MO_in;
            end Set2;

            x := Find_Min(Z.all(1..H));

            T5.SetX6(x);

            accept SetX5(x_new: in Integer) do
                x := x_new;
            end SetX5;

            Calc(A.all(1..H), B.all(1..H), d, x, E.all, MO.all(1..H), MK.all);

            T5.SetA6(A.all);

            Put_Line(":> Finished Task 6");
        end T6;
-------------------------------------------------------------------------------
        task body T7
        is
            A: access Vector_2H := new Vector_2H;
            B: access Vector_H := new Vector_H;
            d: Integer;
            Z: access Vector_4H := new Vector_4H;
            E: access Vector_N := new Vector_N;
            MO: access Matrix_H := new Matrix_H;
            MK: access Matrix_N := new Matrix_N;
            x: Integer;
        begin
            Put_Line(":> Starting Task 7...");

            accept Set8(Z_in: in Vector_4H; E_in: in Vector_N) do
                Z.all := Z_in;
                E.all := E_in;
            end Set8;

            T5.Set7(Z.all(2*H+1..4*H), E.all);
            T3.Set7(Z.all(H+1..2*H), E.all);

            accept Set3(B_in: in Vector_H; MK_in: in Matrix_N; d_in: Integer; MO_in: in Matrix_H) do
                B.all := B_in;
                MK.all := MK_in;
                d := d_in;
                MO.all := MO_in;
            end Set3;

            x := Find_Min(Z.all(1..H));

            accept SetX8(x_other: in Integer) do
                if (x_other < x) then
                    x := x_other;
                end if;
            end SetX8;

            T5.SetX7(x);

            accept SetX5(x_new: in Integer) do
                x := x_new;
            end SetX5;

            T8.SetX7(x);

            Calc(A.all(1..H), B.all(1..H), d, x, E.all, MO.all(1..H), MK.all);

            accept SetA8(A_in: in Vector_H) do
                A.all(H+1..2*H) := A_in;
            end SetA8;

            T5.SetA7(A.all);

            Put_Line(":> Finished Task 7");
        end T7;
-------------------------------------------------------------------------------
        task body T8
        is
            A: access Vector_H := new Vector_H;
            B: access Vector_H := new Vector_H;
            d: Integer;
            Z: access Vector_N := new Vector_N;
            E: access Vector_N := new Vector_N;
            MO: access Matrix_H := new Matrix_H;
            MK: access Matrix_N := new Matrix_N;
            x: Integer;
        begin
            Put_Line(":> Starting Task 8...");

            Fill_Vector_Ones(Z);
            Fill_Vector_Ones(E);

            T7.Set8(Z.all(4*H+1..8*H), E.all);
            T6.Set8(Z.all(2*H+1..4*H), E.all);
            T4.Set8(Z.all(H+1..2*H), E.all);

            accept Set4(B_in: in Vector_H; MK_in: in Matrix_N; d_in: Integer; MO_in: in Matrix_H) do
                B.all := B_in;
                MK.all := MK_in;
                d := d_in;
                MO.all := MO_in;
            end Set4;

            x := Find_Min(Z.all(1..H));

            T7.SetX8(x);

            accept SetX7(x_new: in Integer) do
                x := x_new;
            end SetX7;

            Calc(A.all(1..H), B.all(1..H), d, x, E.all, MO.all(1..H), MK.all);

            T7.SetA8(A.all);

            Put_Line(":> Finished Task 8");
        end T8;
-------------------------------------------------------------------------------

    begin
        null;
    end Tasks_Start;
begin
    Tasks_Start;
end Main;
