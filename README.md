# HW4. Threads
Кузнецов Максим Вадимович. БПИ219. Вариант 6

Задание выполнялось по критериям на оценку 9.

## Условие
Задача о каннибалах. Племя из n дикарей ест вместе из большого горшка, который вмещает m кусков тушеного миссионера. Когда дикарь хочет обедать, он ест из горшка один кусок, если только горшок не пуст, иначе дикарь будит повара и ждет, пока тот не наполнит горшок. Повар, сварив обед, засыпает. Создать многопоточное приложение, моделирующее обед дикарей. При решении задачи пользоваться семафорами.

## Модель параллельных вычислений
Производители и потребители. В данной задаче в качестве произдодителей выступает повар (их может быть несколько, но для соответствия условию сделан только один). В качестве потребителей - каннибалы. Повар наполняет горшок с едой (переменная potValue становится равной potCapacity), а каждый каннибыл уменьшает значение еды в горшке на 1. Таким образом, получается модифицированная версия задачи о кольцевом буфере, где в вместо массива, в который добавляются значения, используется переменная, хранящая количество еды в горшке.

## Входные данные программы
n и m - количество дикарей и вместительность горшка с едой соответственно. Каждое из этих значений должно быть целым числом не меньше 1, иначе задача не имеет смысла. При вводе некорректного значения, программа попросит ввести число заново. Также выводятся подсказки везде, где это необходимо.

## Сценарий, описывающий поведение объектов разрабатываемой программы
В программе есть 3 типа сущностей: каннибалы, повар, горшок. У горшка есть 2 свойства: вместительность и текущее количество еды. У повара есть 2 состояния: готовит еду или спит. В начале программы горшок пуст, поэтому повар готовит еду до тех пор, пока количество еды в горшке не будет соответствовать его вместительности. Затем повар засыпает. Далее каннибалы начинают одновременно есть еду из горшка. Если еды меньше, чем количество каннибалов, то съесть еду успеют не все. После того, как конкретный каннибал съел один кусок, он считает себя наевшимся и делает перерыв. Если еда после этого осталась, он продолжает есть. Так до тех пор, пока горшок не опустеет. Первый каннибал, заметивший что горшок пуст, будит повара. Повар готовит еду и засыпает. Затем всё повторяется.

## Обобщенный алгоритм
- Горшок в программе представлен в виде 2х глобальных переменных: potCapacity (вместительность) и potValue (текущее количество еды). Вместительность задает пользователь при вводе, а potValue изменяют потоки повара и каннибалов (именно поэтому переменная сделана глобальной).
- Повар представлен в виде потока, выполняюяющего функцию Chef, и глобальной переменной isChefSleeping, которая хранит true, если повар спит, и false в обратном случае. Также у повара есть его номер (pNum), на случай дальнейшей модификации программы с использованием нескольких поваров.
- Каждый каннибал представлен в виде потока, выполняющего функцию Cannibal. У каннибала есть его номер (cNum), чтобы отличать его от других каннибалов при выводе результата работы программы.
- Выполнение функций каннибалов и повара происходит в цикле while(1), то есть эти две сущности "живут" до тех пор, пока мы не остановим выполнение программы.

В самом начале программы запускаются потоки повара и каннибалов. Далее основной поток (main) отправляет через семафор сигнал повару о том, что можно начинать готовить еду. Повар уменьшает значение семафора на 1 (то есть заставляет ждать другие потоки) и, если не спит, готовит еду: присваевает переменной potValue значение potCapacity. К тому же, для имитации процесса приготовления используется задержка sleep(1). Затем повар увеличивает значение семафора на 1, позволяя потокам каннибалов продолжить выполнение своих функций. Далее каннибал уменьшает значение семафора на 1 (запрещая остальным взаимодействовать с горшком с едой) и проверяет, что в горшке есть еда и что повар спит. Если условия выполняются, что он уменьшает на 1 количество кусков в горшке (--potValue). Если нет, то он будит повара (isChefSleeping = false). Затем он увеличивает значение семафора на 1, чтобы дать другим потокам возможность закончить выполнение программы. Далее таким же образом отрабатывают другие потоки каннибалов, и всё начинается заново.

## Результаты тестов
