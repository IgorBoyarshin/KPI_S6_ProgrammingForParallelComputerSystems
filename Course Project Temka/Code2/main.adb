-------------------------------------------------------------------------------
-- Course Project: Ada. Randezvous
-- MA = MB * MC + min(Z) * MK
-- By Artem Berezynec
-- 13.05.2018
-------------------------------------------------------------------------------
with Ada.Text_IO, Ada.Integer_Text_IO, Ada.Synchronous_Task_Control, System.Multiprocessors, Data, Ada.Calendar;
use Ada.Text_IO, Ada.Integer_Text_IO, Ada.Synchronous_Task_Control, System.Multiprocessors, Ada.Calendar;

procedure Main
is
    N: constant := 600;
    P: constant := 4;
    H: constant := N / P;
    Memory: constant := 4000000;
    Output_Threshold: constant := 8;

    package My_Data is new Data(N, H);
    use My_Data;

    start: Time;
    time: Duration;

-------------------------------------------------------------------------------
    procedure Tasks_Start is
        -- Tasks specification
        task T1 is
            pragma Task_Name("Task 1");
            pragma Storage_Size(Memory);

            entry Set_MC_Z_MK(MC_in: in Matrix_N; Z_in: in Vector_H; MK_in: in Matrix_H);
            entry SetMA(MA_in: in Matrix_3H);
            entry SetMin(x_in: in INteger);
        end T1;
        task T2 is
            pragma Task_Name("Task 2");
            pragma Storage_Size(Memory);

            entry Set_MB(MB_in: in Matrix_3H);
            entry Set_MC_Z_MK(MC_in: in Matrix_N; Z_in: in Vector_2H; MK_in: in Matrix_2H);
            entry SetMA(MA_in: in Matrix_2H);
            entry SetX(x_in: in Integer);
            entry SetMin(x_in: in INteger);
        end T2;
        task T3 is
            pragma Task_Name("Task 3");
            pragma Storage_Size(Memory);

            entry Set_MK(MK_in: in Matrix_3H);
            entry Set_MB(MB_in: in Matrix_2H);
            entry SetMA(MA_in: in Matrix_H);
            entry SetX(x_in: in Integer);
        end T3;
        task T4 is
            pragma Task_Name("Task 4");
            pragma Storage_Size(Memory);

            entry Set_MB_MC_Z(MB_in: in Matrix_H; MC_in: in Matrix_N; Z_in: in Vector_H);
            entry SetMin(x_in: in Integer);
        end T4;
        -- Tasks implementation
-------------------------------------------------------------------------------
        task body T1
        is
            MA: access Matrix_N := new Matrix_N;
            MB: access Matrix_N := new Matrix_N;
            MC: access Matrix_N := new Matrix_N;
            Z: access Vector_H := new Vector_H;
            MK: access Matrix_H := new Matrix_H;
            x: Integer;
        begin
            Put_Line("Starting Task 1...");

            Fill_Matrix_Ones(MB);

            T2.Set_MB(MB.all(H+1..4*H));

            accept Set_MC_Z_MK(MC_in: in Matrix_N; Z_in: in Vector_H; MK_in: in Matrix_H) do
                MC.all := MC_in;
                Z.all := Z_in;
                MK.all := MK_in;
            end Set_MC_Z_MK;

            x := Find_Min(Z.all);

            T2.SetX(x);

            accept SetMin(x_in: in Integer) do
                x := x_in;
            end SetMin;

            Calc(MA.all(1..H), MB.all(1..H), MC.all, x, MK(1..H));

            accept SetMA(MA_in: in Matrix_3H) do
                MA.all(H+1..4*H) := MA_in;
            end SetMA;

            if (N <= Output_Threshold) then
                Output_Matrix(MA);
            end if;

            Put_Line("Finished Task 1");
        end T1;
-------------------------------------------------------------------------------
        task body T2
        is
            MA: access Matrix_3H := new Matrix_3H;
            MB: access Matrix_3H := new Matrix_3H;
            MC: access Matrix_N := new Matrix_N;
            Z: access Vector_2H := new Vector_2H;
            MK: access Matrix_2H := new Matrix_2H;
            x: Integer;
        begin
            Put_Line("Starting Task 2...");

            accept Set_MB(MB_in: in Matrix_3H) do
                MB.all := MB_in;
            end Set_MB;

            accept Set_MC_Z_MK(MC_in: in Matrix_N; Z_in: in Vector_2H; MK_in: in Matrix_2H) do
                MC.all := MC_in;
                Z.all := Z_in;
                MK.all := MK_in;
            end Set_MC_Z_MK;

            T3.Set_MB(MB.all(H+1..3*H));
            T1.Set_MC_Z_MK(MC.all, Z.all(1..H), MK.all(1..H));

            x := Find_Min(Z.all(H+1..2*H));

            accept SetX(x_in: in Integer) do
                if x_in < x then
                    x := x_in;
                end if;
            end SetX;

            T3.SetX(x);

            accept SetMin(x_in: in Integer) do
                x := x_in;
            end SetMin;

            T1.SetMin(x);

            Calc(MA.all(1..H), MB.all(1..H), MC.all, x, MK(H+1..2*H));

            accept SetMA(MA_in: in Matrix_2H) do
                MA.all(H+1..3*H) := MA_in;
            end SetMA;

            T1.SetMA(MA.all);

            Put_Line("Finished Task 2");
        end T2;
-------------------------------------------------------------------------------
        task body T3
        is
            MA: access Matrix_2H := new Matrix_2H;
            MB: access Matrix_2H := new Matrix_2H;
            MC: access Matrix_N := new Matrix_N;
            Z: access Vector_N := new Vector_N;
            MK: access Matrix_3H := new Matrix_3H;
            x: Integer;
        begin
            Put_Line("Starting Task 3...");

            Fill_Matrix_Ones(MC);
            Fill_Vector_Ones(Z);

            accept Set_MK(MK_in: in Matrix_3H) do
                MK.all := MK_in;
            end Set_MK;

            T2.Set_MC_Z_MK(MC.all, Z.all(1..2*H), MK(1..2*H));

            accept Set_MB(MB_in: in Matrix_2H) do
                MB.all := MB_in;
            end Set_MB;

            T4.Set_MB_MC_Z(MB.all(H+1..2*H), MC.all, Z.all(3*H+1..4*H));

            x := Find_Min(Z.all(2*H+1..3*H));

            accept SetX(x_in: in Integer) do
                if x_in < x then
                    x := x_in;
                end if;
            end SetX;
            accept SetX(x_in: in Integer) do
                if x_in < x then
                    x := x_in;
                end if;
            end SetX;

            T2.SetMin(x);
            T4.SetMin(x);

            Calc(MA.all(1..H), MB.all(1..H), MC.all, x, MK(2*H+1..3*H));

            accept SetMA(MA_in: in Matrix_H) do
                MA.all(H+1..2*H) := MA_in;
            end SetMA;

            T2.SetMA(MA.all);

            Put_Line("Finished Task 3");
        end T3;
-------------------------------------------------------------------------------
        task body T4
        is
            MA: access Matrix_H := new Matrix_H;
            MB: access Matrix_H := new Matrix_H;
            MC: access Matrix_N := new Matrix_N;
            Z: access Vector_H := new Vector_H;
            MK: access Matrix_N := new Matrix_N;
            x: Integer;
        begin
            Put_Line("Starting Task 4...");

            Fill_Matrix_Ones(MK);

            T3.Set_MK(MK.all(1..3*H));

            accept Set_MB_MC_Z(MB_in: in Matrix_H; MC_in: in Matrix_N; Z_in: in Vector_H) do
                MB.all := MB_in;
                MC.all := MC_in;
                Z.all := Z_in;
            end Set_MB_MC_Z;

            x := Find_Min(Z.all(1..H));

            T3.SetX(x);

            accept SetMin(x_in: in Integer) do
                if x_in < x then
                    x := x_in;
                end if;
            end SetMin;

            Calc(MA.all(1..H), MB.all(1..H), MC.all, x, MK(3*H+1..4*H));

            T3.SetMA(MA.all);

            Put_Line("Finished Task 4");
        end T4;
-------------------------------------------------------------------------------
    begin
        null;
    end Tasks_Start;
begin
    start := clock;
    Tasks_Start;
    time := clock - start;
    delay 1.0;
    Put_Line("Elapsed time = " & Duration'Image(time));
end Main;
