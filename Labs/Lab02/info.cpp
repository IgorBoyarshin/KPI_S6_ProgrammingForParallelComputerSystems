// Semaphores
// 0 -- not signaled
// >=1 -- signaled
HANDLE Sem1;
Sem1 = CreateSemaphore(
    NULL,   // default security attributes
    0,      // initial count
    3,      // maximum count
    NULL    // unnamed semaphore
);
CloseHandle(Sem1);

ReleaseSemaphore( // INC (signal)
    Sem1,   // semaphore
    4,      // by which count to increase
    NULL    // pointer to previous count
);
WaitForSingleObject( // DEC
    Sem1,   // semaphore
    INFINITE// time-out interval. INFINITE => return only when signaled
);
WaitForMultipleObjects(
    4,      // array size
    handles,// array of handles
    TRUE,   // TRUE => wait for ALL signaled, otherwise for ANY
    INFINITE// time-out interval
);

// Mutexes
HANDLE Mut1;
Mut1 = CreateMutex(
    NULL,   // default security attributes
    FALSE,  // initially not owned
    NULL    // unnamed
);

WaitForSingleObject(...); // request ownership
ReleaseMutex(Mut1);

// Critical Sections
CRITICAL_SECTION Cs1;
InitializeCriticalSection(&Cs1);
DeleteCriticalSection(&Cs1);

EnterCriticalSection(&Cs1);
LeaveCriticalSection(&Cs1);

// Events
HANDLE Ev1;
Ev1 = CreateEvent(
    NULL,   // default security attibutes
    FALSE,  // TRUE => manual-reset event, FALSE => automatic
            // (reset when single waiting thread has been released).
    FALSE,   // FALSE => initial - non-signaled, TRUE => signaled
    NULL    // unnamed event
);
CloseHandle(Ev1);

WaitForSingleObject(...);
SetEvent( // set to signaled
    Ev1     // event
);
