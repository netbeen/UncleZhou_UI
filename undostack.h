#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <stack>
#include <QImage>

class UndoStack
{
public:
    static UndoStack* getInstance();
    void init(QImage initImage);
    void push(QImage historyImage);
    QImage pop();

private:
    static UndoStack* ptr2UndoStack;
    UndoStack();
    std::stack<QImage> historyStack;
    QImage initImage;
};

#endif // UNDOSTACK_H
