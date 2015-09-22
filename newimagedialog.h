#ifndef NEWIMAGEDIALOG_H
#define NEWIMAGEDIALOG_H

#include <QWidget>
#include <QDialog>

class NewImageDialog : public QDialog
{
    Q_OBJECT
public:
    NewImageDialog(QDialog* parent=0);

private:
    void initDialogLayout();

signals:

};

#endif // NEWIMAGEDIALOG_H
