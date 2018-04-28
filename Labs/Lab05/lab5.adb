-------------------------------------------------------------------------------
-- Lab5: Ada. Protected Modeules
-- Task: (B*C)*Z + min(E) * T * (MO*MK)
-- Author: Igor Boyarshin
-- Date: 14.04.2018
-------------------------------------------------------------------------------
with Ada.Text_IO, Ada.Integer_Text_IO, Ada.Synchronous_Task_Control, System.Multiprocessors, Data;
use Ada.Text_IO, Ada.Integer_Text_IO, Ada.Synchronous_Task_Control, System.Multiprocessors;

procedure Lab5
is
    N: constant := 400;
    P: constant := 4;
    H: constant := N / P;
    Memory: constant := 5000000;
    Output_Threshold: constant := 8;

    package My_Data is new Data(N);
    use My_Data;

    A: access Vector := new Vector;
    B: access Vector := new Vector;
    C: access Vector := new Vector;
    Z: access Vector := new Vector;
    E: access Vector := new Vector;
    MO: access Matrix := new Matrix;

-------------------------------------------------------------------------------
    -- Protected Module Specification
    protected Box is
        procedure UpdateA(x: in Integer);
        function GetA return Integer;
        procedure UpdateE(x: in Integer);
        function GetE return Integer;
        procedure SetMK(M: access Matrix);
        function GetMK return access Matrix;
        procedure SetT(V: access Vector);
        function GetT return access Vector;
        procedure SignalInputEnd;
        procedure SignalCalc1End;
        procedure SignalCalc2End;
        entry WaitInputEnd;
        entry WaitCalc1End;
        entry WaitCalc2End;

        private
            a: Integer := 0;
            e: Integer := 32000;
            MK: access Matrix := new Matrix;
            T: access Vector := new Vector;
            F1: Integer := 0;
            F2: Integer := 0;
            F3: Integer := 0;
    end Box;

    -- Protected Module Implementation
    protected body Box is
        procedure UpdateA(x: in Integer) is
        begin
            a := a + x;
        end UpdateA;

        function GetA return Integer is
        begin
            return a;
        end GetA;

        procedure UpdateE(x: in Integer) is
        begin
            if (x < e) then
                e := x;
            end if;
        end UpdateE;

        function GetE return Integer is
        begin
            return e;
        end GetE;

        procedure SetMK(M: access Matrix) is
        begin
            MK := M;
        end SetMK;

        function GetMK return access Matrix is
        begin
            return Copy_Matrix(MK);
        end;

        procedure SetT(V: access Vector) is
        begin
            T := V;
        end SetT;

        function GetT return access Vector is
        begin
            return Copy_Vector(T);
        end GetT;

        procedure SignalInputEnd is
        begin
            F1 := F1 + 1;
        end SignalInputEnd;

        procedure SignalCalc1End is
        begin
            F2 := F2 + 1;
        end SignalCalc1End;

        procedure SignalCalc2End is
        begin
            F3 := F3 + 1;
        end SignalCalc2End;

        entry WaitInputEnd when F1 = 4 is
        begin
            null;
        end WaitInputEnd;

        entry WaitCalc1End when F2 = 4 is
        begin
            null;
        end WaitCalc1End;

        entry WaitCalc2End when F3 = 3 is
        begin
            null;
        end WaitCalc2End;
    end Box;
-------------------------------------------------------------------------------
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
        task T3 is
            pragma Task_Name("Task 3");
            pragma Storage_Size(Memory);
        end T3;
        task T4 is
            pragma Task_Name("Task 4");
            pragma Storage_Size(Memory);
        end T4;
-------------------------------------------------------------------------------
        -- Tasks implementation
        task body T1
        is
            low, high: Integer;
            temp, prod: Integer;
            ai, ei: Integer;
            Ti: access Vector := new Vector;
            MKi: access Matrix;
        begin
            Put_Line(":> Starting Task 1...");
            low := 1;
            high := H;

            -- Input
            Fill_Vector_Ones(Ti);
            Box.SetT(Ti);
            Fill_Vector_Ones(B);

            -- Synchronize on input
            Box.SignalInputEnd;
            Box.WaitInputEnd;

            -- Calculations1 A
            ai := 0;
            for i in low..high loop
                ai := ai + B(i) * C(i);
            end loop;

            Box.UpdateA(ai);

            -- Calculations1 E
            ei := E(low);
            for i in low..high loop
                if (E(i) < ei) then
                    ei := E(i);
                end if;
            end loop;

            Box.UpdateE(ei);

            -- Synchronize on Calculations1
            Box.SignalCalc1End;
            Box.WaitCalc1End;

            -- Copies
            ai := Box.GetA;
            ei := Box.GetE;
            MKi := Box.GetMK;
            Ti := Box.GetT;

            -- Calculations 2
            for h in low..high loop
                temp := 0;
                for i in 1..N loop
                    prod := 0;
                    for j in 1..N loop
                        prod := prod + MO(h)(j) * MKi(j)(i);
                    end loop;

                    temp := temp + Ti(i) * prod;
                end loop;

                A(h) := ai * Z(h) + ei * temp;
            end loop;

            -- Synchronize on Calculations2
            Box.SignalCalc2End;

            Put_Line(":> Finished Task 1");
        end T1;
-------------------------------------------------------------------------------
        task body T2
        is
            low, high: Integer;
            temp, prod: Integer;
            ai, ei: Integer;
            Ti: access Vector;
            MKi: access Matrix;
        begin
            Put_Line(":> Starting Task 2...");
            low := H + 1;
            high := 2*H;

            -- Input
            Fill_Vector_Ones(C);
            for i in 1..N loop
                A(i) := 0;
            end loop;

            -- Synchronize on input
            Box.SignalInputEnd;
            Box.WaitInputEnd;

            -- Calculations1 A
            ai := 0;
            for i in low..high loop
                ai := ai + B(i) * C(i);
            end loop;

            Box.UpdateA(ai);

            -- Calculations1 E
            ei := E(low);
            for i in low..high loop
                if (E(i) < ei) then
                    ei := E(i);
                end if;
            end loop;

            Box.UpdateE(ei);

            -- Synchronize on Calculations1
            Box.SignalCalc1End;
            Box.WaitCalc1End;

            -- Copies
            ai := Box.GetA;
            ei := Box.GetE;
            MKi := Box.GetMK;
            Ti := Box.GetT;

            -- Calculations 2
            for h in low..high loop
                temp := 0;
                for i in 1..N loop
                    prod := 0;
                    for j in 1..N loop
                        prod := prod + MO(h)(j) * MKi(j)(i);
                    end loop;

                    temp := temp + Ti(i) * prod;
                end loop;

                A(h) := ai * Z(h) + ei * temp;
            end loop;

            -- Synchronize on Calculations2
            Box.WaitCalc2End;

            -- Output
            if (N <= Output_Threshold) then
                Output_Vector(A);
            end if;

            Put_Line(":> Finished Task 2");
        end T2;
-------------------------------------------------------------------------------
        task body T3
        is
            low, high: Integer;
            temp, prod: Integer;
            ai, ei: Integer;
            Ti: access Vector;
            MKi: access Matrix := new Matrix;
        begin
            Put_Line(":> Starting Task 3...");
            low := 2*H + 1;
            high := 3*H;

            -- Input
            Fill_Vector_Ones(Z);
            Fill_Matrix_Ones(MKi);
            Box.SetMK(MKi);

            -- Synchronize on input
            Box.SignalInputEnd;
            Box.WaitInputEnd;

            -- Calculations1 A
            ai := 0;
            for i in low..high loop
                ai := ai + B(i) * C(i);
            end loop;

            Box.UpdateA(ai);

            -- Calculations1 E
            ei := E(low);
            for i in low..high loop
                if (E(i) < ei) then
                    ei := E(i);
                end if;
            end loop;

            Box.UpdateE(ei);

            -- Synchronize on Calculations1
            Box.SignalCalc1End;
            Box.WaitCalc1End;

            -- Copies
            ai := Box.GetA;
            ei := Box.GetE;
            MKi := Box.GetMK;
            Ti := Box.GetT;

            -- Calculations 2
            for h in low..high loop
                temp := 0;
                for i in 1..N loop
                    prod := 0;
                    for j in 1..N loop
                        prod := prod + MO(h)(j) * MKi(j)(i);
                    end loop;

                    temp := temp + Ti(i) * prod;
                end loop;

                A(h) := ai * Z(h) + ei * temp;
            end loop;

            -- Synchronize on Calculations2
            Box.SignalCalc2End;

            Put_Line(":> Finished Task 3");
        end T3;
-------------------------------------------------------------------------------
        task body T4
        is
            low, high: Integer;
            temp, prod: Integer;
            ai, ei: Integer;
            Ti: access Vector;
            MKi: access Matrix;
        begin
            Put_Line(":> Starting Task 4...");
            low := 3*H + 1;
            high := 4*H;

            -- Input
            Fill_Vector_Ones(E);
            E(2) := -10;
            Fill_Matrix_Ones(MO);

            -- Synchronize on input
            Box.SignalInputEnd;
            Box.WaitInputEnd;

            -- Calculations1 A
            ai := 0;
            for i in low..high loop
                ai := ai + B(i) * C(i);
            end loop;

            Box.UpdateA(ai);

            -- Calculations1 E
            ei := E(low);
            for i in low..high loop
                if (E(i) < ei) then
                    ei := E(i);
                end if;
            end loop;

            Box.UpdateE(ei);

            -- Synchronize on Calculations1
            Box.SignalCalc1End;
            Box.WaitCalc1End;

            -- Copies
            ai := Box.GetA;
            ei := Box.GetE;
            MKi := Box.GetMK;
            Ti := Box.GetT;

            -- Calculations 2
            for h in low..high loop
                temp := 0;
                for i in 1..N loop
                    prod := 0;
                    for j in 1..N loop
                        prod := prod + MO(h)(j) * MKi(j)(i);
                    end loop;

                    temp := temp + Ti(i) * prod;
                end loop;

                A(h) := ai * Z(h) + ei * temp;
            end loop;

            -- Synchronize on Calculations2
            Box.SignalCalc2End;

            Put_Line(":> Finished Task 4");
        end T4;
-------------------------------------------------------------------------------

    begin
        null;
    end Tasks_Start;
begin
    Tasks_Start;
end Lab5;
