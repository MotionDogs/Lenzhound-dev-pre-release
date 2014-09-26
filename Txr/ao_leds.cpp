#include "qp_port.h"
#include "bsp.h"
#include "Txr.h"

Q_DEFINE_THIS_FILE

using namespace QP;


class Blinky : public QP::QActive {
public:
    QTimeEvt m_timeEvt;

public:
    Blinky()
      : QActive(Q_STATE_CAST(&Blinky::initial)),
        m_timeEvt(FLASH_RATE_SIG)    {
        // empty
    }

protected:
    static QP::QState initial(Blinky * const me, QP::QEvt const * const e);
    static QP::QState off(Blinky * const me, QP::QEvt const * const e);
    static QP::QState on(Blinky * const me, QP::QEvt const * const e);
};


//${AOs::Blinky::SM::initial} .................................................
QP::QState Blinky::initial(Blinky * const me, QP::QEvt const * const e) {
    me->m_timeEvt.postEvery(me, BSP_TICKS_PER_SEC/2);
    return Q_TRAN(&off);
}


//${AOs::Blinky::SM::off} ....................................................
QP::QState Blinky::off(Blinky * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Blinky::SM::off}
        case Q_ENTRY_SIG: {
            GREEN2_LED_OFF();
            status_ = Q_HANDLED();
            break;
        }
        // ${AOs::Blinky::SM::off::TIMEOUT}
        case FLASH_RATE_SIG: {
            status_ = Q_TRAN(&on);
            break;
        }
        default: {
            status_ = Q_SUPER(&QP::QHsm::top);
            break;
        }
    }
    return status_;
}
//${AOs::Blinky::SM::on} .....................................................
QP::QState Blinky::on(Blinky * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Blinky::SM::on}
        case Q_ENTRY_SIG: {
            GREEN2_LED_ON();
            status_ = Q_HANDLED();
            break;
        }
        // ${AOs::Blinky::SM::on::TIMEOUT}
        case FLASH_RATE_SIG: {
            status_ = Q_TRAN(&off);
            break;
        }
        default: {
            status_ = Q_SUPER(&QP::QHsm::top);
            break;
        }
    }
    return status_;
}


//............................................................................
static Blinky l_blinky;                           // instantiate the Blinky AO
QActive * const AO_Blinky = &l_blinky;     // initialize the global pointer to Blinky


