# ИДЗ 4
<b>Выполнила:</b> Панфилова Мария \
<b>Группа:</b> БПИ226 \
<b>Вриант:</b> 1 \
<b>Желаемая оценка:</b> 8 \
Все программы на соответствующую оценку лежат в соответствующих папках.
## Текст варианта
Задача о парикмахере. В тихом городке есть парикмахерская. Салон парикмахерской мал, работать в нем нем может только один парикмахер, обслуживающий одного посетителя. Есть несколько стульев для ожидания в очереди. Парикмахер всю жизнь обслуживает посетителей. Когда в салоне никого нет, он спит в кресле. Когда посетитель приходит и видит спящего парикмахера, он будет его, садится в кресло, и сидит в нем, пока парикмахер обслуживает его. Если посетитель приходит, а парикмахер занят, то он встает в очередь, садится на свободный стул и «засыпает». После стрижки парикмахер сам провожает посетителя. Если есть ожидающие посетители, то парикмахер будит одного из них, ждет пока тот сядет в кресло парикмахера и начинает стрижку. Если никого нет, он снова садится в свое кресло и засыпает до прихода посетителя.  Количество стульев для ожидания в очереди ограничено числом N. Если стульев не хватает, то пришедший посетитель уходит.
Парикмахера и парикмахерскую моделировать в виде сервера. Каждого из посетителей моделировать в виде отдельного клиента. Клиенты могут запускаться независимо друг от друга как вручную, так и с использованием скриптов, или других программ, запускающих их в фоновом режиме. В этом случае предусмотреть задержку, задающую паузу между запуском клиентов.

(последняя часть условия была проинтерпретированна так: клиентами (по лору задачи) могут быть отделыне процессы которые посылают 1 запрос на сервер о том что они пришли. Тк иначе эта задача требует сразу решить себя на 8 баллов) )

## Сценарий
*Генератор клиентов парихмахерской* каждые **K** секунд содаёт процесс клиента, который отправляет запрос в на сервер (идет в парихмахерскую), сожержащий его pid. После отправки запроса клиент ждёт ответ, если ответ положительный клиент получил обслуживание, иначе все места в очереди в парихмахерскую были заняты и клиент ушел. После этого поток завершает работу (клиент уходит). \
*Парихмахерская* или сервер принимает запросы от клиентов парихмахерской и заполняет очередь. Если для текущего клиента нет места в очереди ему отправляется сообщение о переполненой очереди. Далее парихмахер работает с клиентами и после каждого клиента обновляет очередь. После работы с клиентом сервер отправляет сообщение об оказаном сервисе. \
В вариации задания с логгером добавляется еще *сервер логер*. Он выводит на экран содержимое запросов от всех его клиентов. Сервер парихмахер после каждого действия посылает тут запрос с текущим статусом.

## Входные данные
Все входные данные передаются в качестве аргументов командной строки.
- Сервер логгер: номер порта
- Сервер парихмахер: номер порта, время работы парихмахера с одним клиентом, количество мест в очереди, (на 6 и выше IP логгера, порт логгера)
- Клиент: IP парихмахера, время в секундах между созданиями новых клиентов, порт сервера парихмахера

## Решение на 4-5
Реализован сценарий описанный в разделе сценарий, без учета логгера.


## Решение на 6-7
Добавлено взаимодействие сервера парихмахера и сервера логгера как в сценарии.

## Ренание 8
По факту это решение на 6-7, так как используя протокол UDP, сервер не устанавливает связи между собой и конкретным клиентом. Поэтому программа поддерживает работу с Несколькими клиентами одновременно.

![image](https://github.com/MShpiz/OS_IHW_3/blob/main/IMG_20240520_122506_815.jpg)

