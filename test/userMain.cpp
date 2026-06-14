/*#include "printing.hpp"

#define LEVEL_1_IMPLEMENTED 0
#define LEVEL_2_IMPLEMENTED 1
#define LEVEL_3_IMPLEMENTED 1
#define LEVEL_4_IMPLEMENTED 0

#if LEVEL_2_IMPLEMENTED == 1
// TEST 1 (zadatak 2, niti C API i sinhrona promena konteksta)
#include "../test/Threads_C_API_test.hpp"
// TEST 2 (zadatak 2., niti CPP API i sinhrona promena konteksta)
#include "../test/Threads_CPP_API_test.hpp"
// TEST 7 (zadatak 2., testiranje da li se korisnicki kod izvrsava u korisnickom rezimu)
//#include "../test/System_Mode_test.hpp"
#endif

#if LEVEL_3_IMPLEMENTED == 1
// TEST 3 (zadatak 3., kompletan C API sa semaforima, sinhrona promena konteksta)
#include "../test/ConsumerProducer_C_API_test.hpp"
// TEST 4 (zadatak 3., kompletan CPP API sa semaforima, sinhrona promena konteksta)
#include "../test/ConsumerProducer_CPP_Sync_API_test.hpp"
#endif

#if LEVEL_4_IMPLEMENTED == 1
// TEST 5 (zadatak 4., thread_sleep test C API)
#include "../test/ThreadSleep_C_API_test.hpp"
// TEST 6 (zadatak 4. CPP API i asinhrona promena konteksta)
#include "../test/ConsumerProducer_CPP_API_test.hpp"
//#include "System_Mode_test.hpp"

#endif

void userMain() {
    printString("Unesite broj testa? [1-7]\n");
    int test = getc() - '0';
    getc(); // Enter posle broja

    if ((test >= 1 && test <= 2) || test == 7) {
        if (LEVEL_2_IMPLEMENTED == 0) {
            printString("Nije navedeno da je zadatak 2 implementiran\n");
            return;
        }
    }

    if (test >= 3 && test <= 4) {
        if (LEVEL_3_IMPLEMENTED == 0) {
            printString("Nije navedeno da je zadatak 3 implementiran\n");
            return;
        }
    }

    if (test >= 5 && test <= 6) {
        if (LEVEL_4_IMPLEMENTED == 0) {
            printString("Nije navedeno da je zadatak 4 implementiran\n");
            return;
        }
    }

    switch (test) {
        case 1:
#if LEVEL_2_IMPLEMENTED == 1
            Threads_C_API_test();
            printString("TEST 1 (zadatak 2, niti C API i sinhrona promena konteksta)\n");
#endif
            break;
        case 2:
#if LEVEL_2_IMPLEMENTED == 1
            Threads_CPP_API_test();
            printString("TEST 2 (zadatak 2., niti CPP API i sinhrona promena konteksta)\n");
#endif
            break;
        case 3:
#if LEVEL_3_IMPLEMENTED == 1
            producerConsumer_C_API();
            printString("TEST 3 (zadatak 3., kompletan C API sa semaforima, sinhrona promena konteksta)\n");
#endif
            break;
        case 4:
#if LEVEL_3_IMPLEMENTED == 1
            producerConsumer_CPP_Sync_API();
            printString("TEST 4 (zadatak 3., kompletan CPP API sa semaforima, sinhrona promena konteksta)\n");
#endif
            break;
        case 5:
#if LEVEL_4_IMPLEMENTED == 1
            testSleeping();
            printString("TEST 5 (zadatak 4., thread_sleep test C API)\n");
#endif
            break;
        case 6:
#if LEVEL_4_IMPLEMENTED == 1
            testConsumerProducer();
            printString("TEST 6 (zadatak 4. CPP API i asinhrona promena konteksta)\n");
#endif
            break;
        //case 7: //ovde stavi kosa crta zvezda komentar
#if LEVEL_2_IMPLEMENTED == 1
            System_Mode_test();
            printString("Test se nije uspesno zavrsio\n");
            printString("TEST 7 (zadatak 2., testiranje da li se korisnicki kod izvrsava u korisnickom rezimu)\n");
#endif
            break;*/
        /*default:
            printString("Niste uneli odgovarajuci broj za test\n");
    }
}*/



//tmp


#include "printing.hpp"
#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"

#define LEVEL_1_IMPLEMENTED 0
#define LEVEL_2_IMPLEMENTED 1
#define LEVEL_3_IMPLEMENTED 1
#define LEVEL_4_IMPLEMENTED 0

#if LEVEL_2_IMPLEMENTED == 1
#include "../test/Threads_C_API_test.hpp"
#include "../test/Threads_CPP_API_test.hpp"
#endif

#if LEVEL_3_IMPLEMENTED == 1
#include "../test/ConsumerProducer_C_API_test.hpp"
#include "../test/ConsumerProducer_CPP_Sync_API_test.hpp"
#endif

#if LEVEL_4_IMPLEMENTED == 1
#include "../test/ThreadSleep_C_API_test.hpp"
#include "../test/ConsumerProducer_CPP_API_test.hpp"
#endif

static void printResult(const char* testName, bool ok)
{
    printString(testName);
    printString(ok ? ": PASS\n" : ": FAIL\n");
}

static void dispatchMany(int n)
{
    for (int i = 0; i < n; i++)
    {
        thread_dispatch();
    }
}

static void SemN_CPP_API_test()
{
    printString("\n===== SEMAPHORE CPP waitN/signalN TEST START =====\n");

    Semaphore sem(3);

    int r1 = sem.waitN(2);
    int r2 = sem.waitN(1);
    int r3 = sem.signalN(5);
    int r4 = sem.waitN(5);

    printString("CPP TEST 1 basic waitN/signalN: ");
    if (r1 == 0 && r2 == 0 && r3 == 0 && r4 == 0)
    {
        printString("PASS\n");
    }
    else
    {
        printString("FAIL\n");
    }

    Semaphore semZero(0);

    int r5 = semZero.waitN(0);
    int r6 = semZero.signalN(0);

    printString("CPP TEST 2 zero n: ");
    if (r5 == 0 && r6 == 0)
    {
        printString("PASS\n");
    }
    else
    {
        printString("FAIL\n");
    }

    printString("===== SEMAPHORE CPP waitN/signalN TEST END =====\n\n");
}

/* ========================================================= */
/* sem_wait_n / sem_signal_n test niti                       */
/* ========================================================= */

static sem_t semWait3;
static volatile int wait3Done = 0;
static volatile int wait3Ret = -999;

static void wait3Worker(void*)
{
    printString("wait3Worker: sem_wait_n(3)\n");

    wait3Ret = sem_wait_n(semWait3, 3);

    printString("wait3Worker: ret=");
    printInt(wait3Ret);
    printString("\n");

    wait3Done = 1;
}

static sem_t semMulti;
static volatile int wait2Done = 0;
static volatile int wait3bDone = 0;
static volatile int wait2Ret = -999;
static volatile int wait3bRet = -999;

static void wait2Worker(void*)
{
    printString("wait2Worker: sem_wait_n(2)\n");

    wait2Ret = sem_wait_n(semMulti, 2);

    printString("wait2Worker: ret=");
    printInt(wait2Ret);
    printString("\n");

    wait2Done = 1;
}

static void wait3bWorker(void*)
{
    printString("wait3bWorker: sem_wait_n(3)\n");

    wait3bRet = sem_wait_n(semMulti, 3);

    printString("wait3bWorker: ret=");
    printInt(wait3bRet);
    printString("\n");

    wait3bDone = 1;
}

static sem_t semCloseN;
static volatile int closeDone = 0;
static volatile int closeRet = -999;

static void closeWorker(void*)
{
    printString("closeWorker: sem_wait_n(4)\n");

    closeRet = sem_wait_n(semCloseN, 4);

    printString("closeWorker: ret=");
    printInt(closeRet);
    printString("\n");

    closeDone = 1;
}

static sem_t semFifo;
static volatile int fifoBigDone = 0;
static volatile int fifoSmallDone = 0;
static volatile int fifoBigRet = -999;
static volatile int fifoSmallRet = -999;

static void fifoBigWorker(void*)
{
    printString("fifoBigWorker: sem_wait_n(4)\n");

    fifoBigRet = sem_wait_n(semFifo, 4);

    printString("fifoBigWorker: ret=");
    printInt(fifoBigRet);
    printString("\n");

    fifoBigDone = 1;
}

static void fifoSmallWorker(void*)
{
    printString("fifoSmallWorker: sem_wait_n(1)\n");

    fifoSmallRet = sem_wait_n(semFifo, 1);

    printString("fifoSmallWorker: ret=");
    printInt(fifoSmallRet);
    printString("\n");

    fifoSmallDone = 1;
}

static void SemN_C_API_test()
{
    printString("\n===== SEM_WAIT_N / SEM_SIGNAL_N TEST START =====\n");

    /* TEST 1: odmah ima dovoljno resursa */

    sem_t sem1 = nullptr;

    int retOpen1 = sem_open(&sem1, 3);
    int retWait1 = sem_wait_n(sem1, 2);
    int retWait2 = sem_wait_n(sem1, 1);
    int retSignal1 = sem_signal_n(sem1, 3);
    int retClose1 = sem_close(sem1);

    printResult(
        "TEST 1 immediate wait_n",
        retOpen1 == 0 &&
        retWait1 == 0 &&
        retWait2 == 0 &&
        retSignal1 == 0 &&
        retClose1 == 0
    );

    /* TEST 2: signal_n(2) nije dovoljan, signal_n(1) jeste */

    wait3Done = 0;
    wait3Ret = -999;

    sem_open(&semWait3, 0);

    thread_t tWait3;
    thread_create(&tWait3, wait3Worker, nullptr);

    dispatchMany(10);

    sem_signal_n(semWait3, 2);
    dispatchMany(20);

    bool stillBlockedAfter2 = (wait3Done == 0);

    sem_signal_n(semWait3, 1);

    for (int i = 0; i < 1000 && !wait3Done; i++)
    {
        thread_dispatch();
    }

    bool unblockedAfter3 = (wait3Done == 1 && wait3Ret == 0);

    sem_close(semWait3);

    printResult(
        "TEST 2 blocking until enough resources",
        stillBlockedAfter2 && unblockedAfter3
    );

    /* TEST 3: signal_n(5) budi niti koje cekaju 2 i 3 */

    wait2Done = 0;
    wait3bDone = 0;
    wait2Ret = -999;
    wait3bRet = -999;

    sem_open(&semMulti, 0);

    thread_t tWait2;
    thread_t tWait3b;

    thread_create(&tWait2, wait2Worker, nullptr);
    thread_create(&tWait3b, wait3bWorker, nullptr);

    dispatchMany(20);

    sem_signal_n(semMulti, 5);

    for (int i = 0; i < 2000 && !(wait2Done && wait3bDone); i++)
    {
        thread_dispatch();
    }

    sem_close(semMulti);

    printResult(
        "TEST 3 signal_n unblocks multiple threads",
        wait2Done == 1 &&
        wait3bDone == 1 &&
        wait2Ret == 0 &&
        wait3bRet == 0
    );

    /* TEST 4: sem_close budi blokiranu nit i wait_n vraca gresku */

    closeDone = 0;
    closeRet = -999;

    sem_open(&semCloseN, 0);

    thread_t tClose;
    thread_create(&tClose, closeWorker, nullptr);

    dispatchMany(10);

    sem_close(semCloseN);

    for (int i = 0; i < 1000 && !closeDone; i++)
    {
        thread_dispatch();
    }

    printResult(
        "TEST 4 close wakes blocked wait_n with error",
        closeDone == 1 && closeRet < 0
    );

    /* TEST 5: FIFO test */

    fifoBigDone = 0;
    fifoSmallDone = 0;
    fifoBigRet = -999;
    fifoSmallRet = -999;

    sem_open(&semFifo, 0);

    thread_t tBig;
    thread_t tSmall;

    thread_create(&tBig, fifoBigWorker, nullptr);
    dispatchMany(10);

    thread_create(&tSmall, fifoSmallWorker, nullptr);
    dispatchMany(10);

    sem_signal_n(semFifo, 1);
    dispatchMany(20);

    bool smallDidNotSkipBig = (fifoBigDone == 0 && fifoSmallDone == 0);

    sem_signal_n(semFifo, 3);

    for (int i = 0; i < 1000 && !fifoBigDone; i++)
    {
        thread_dispatch();
    }

    bool bigPassedFirst = (fifoBigRet == 0 && fifoSmallDone == 0);

    sem_signal_n(semFifo, 1);

    for (int i = 0; i < 1000 && !fifoSmallDone; i++)
    {
        thread_dispatch();
    }

    sem_close(semFifo);

    printResult(
        "TEST 5 FIFO order for wait_n",
        smallDidNotSkipBig &&
        bigPassedFirst &&
        fifoSmallDone == 1 &&
        fifoSmallRet == 0
    );

    /* TEST 6: n == 0 i nullptr */

    sem_t semZero = nullptr;
    sem_open(&semZero, 0);

    int retWaitZero = sem_wait_n(semZero, 0);
    int retSignalZero = sem_signal_n(semZero, 0);
    int retNullWait = sem_wait_n(nullptr, 1);
    int retNullSignal = sem_signal_n(nullptr, 1);

    sem_close(semZero);

    printResult(
        "TEST 6 zero and null arguments",
        retWaitZero == 0 &&
        retSignalZero == 0 &&
        retNullWait < 0 &&
        retNullSignal < 0
    );

    printString("===== SEM_WAIT_N / SEM_SIGNAL_N TEST END =====\n\n");
}

/* ========================================================= */
/* Glavni korisnicki main za testiranje                      */
/* ========================================================= */

void userMain()
{
    printString("Unesite broj testa? [1-9]\n");

    int test = getc() - '0';
    getc(); // Enter posle broja

    if ((test >= 1 && test <= 2) || test == 7)
    {
        if (LEVEL_2_IMPLEMENTED == 0)
        {
            printString("Nije navedeno da je zadatak 2 implementiran\n");
            return;
        }
    }

    if (test >= 3 && test <= 4)
    {
        if (LEVEL_3_IMPLEMENTED == 0)
        {
            printString("Nije navedeno da je zadatak 3 implementiran\n");
            return;
        }
    }

    if (test >= 5 && test <= 6)
    {
        if (LEVEL_4_IMPLEMENTED == 0)
        {
            printString("Nije navedeno da je zadatak 4 implementiran\n");
            return;
        }
    }

    switch (test)
    {
        case 1:
#if LEVEL_2_IMPLEMENTED == 1
            Threads_C_API_test();
            printString("TEST 1 (zadatak 2, niti C API i sinhrona promena konteksta)\n");
#endif
            break;

        case 2:
#if LEVEL_2_IMPLEMENTED == 1
            Threads_CPP_API_test();
            printString("TEST 2 (zadatak 2., niti CPP API i sinhrona promena konteksta)\n");
#endif
            break;

        case 3:
#if LEVEL_3_IMPLEMENTED == 1
            producerConsumer_C_API();
            printString("TEST 3 (zadatak 3., kompletan C API sa semaforima, sinhrona promena konteksta)\n");
#endif
            break;

        case 4:
#if LEVEL_3_IMPLEMENTED == 1
            producerConsumer_CPP_Sync_API();
            printString("TEST 4 (zadatak 3., kompletan CPP API sa semaforima, sinhrona promena konteksta)\n");
#endif
            break;

        case 5:
#if LEVEL_4_IMPLEMENTED == 1
            testSleeping();
            printString("TEST 5 (zadatak 4., thread_sleep test C API)\n");
#endif
            break;

        case 6:
#if LEVEL_4_IMPLEMENTED == 1
            testConsumerProducer();
            printString("TEST 6 (zadatak 4. CPP API i asinhrona promena konteksta)\n");
#endif
            break;

        case 8:
            SemN_C_API_test();
            printString("TEST 8 (sem_wait_n i sem_signal_n)\n");
            break;

		case 9:
    		SemN_CPP_API_test();
    		printString("TEST 9 (CPP API za waitN i signalN)\n");
    		break;

        default:
            printString("Niste uneli odgovarajuci broj za test\n");
            break;
    }
}