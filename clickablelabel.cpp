#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags)
    : QLabel(parent) {}

ClickableLabel::~ClickableLabel() {}

/** =============================== **/
/** =========== SLOTS ============= **/
/** =============================== **/

// Slot
void ClickableLabel::mousePressEvent(QMouseEvent*) {
    emit clicked();
}
