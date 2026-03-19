class SerialSendThread(QThread):

    def __init__(self, ser):
        super().__init__()
        self.ser = ser
        self.tx_queue = []
        self.running = True

    def run(self):
        while self.running:
            if self.tx_queue:
                msg = self.tx_queue.pop(0)
                self.ser.write(msg.encode("utf-8"))
            self.msleep(1)  # 极小延迟，不阻塞

    def send(self, msg):
        self.tx_queue.append(msg)

    def stop(self):
        self.running = False
        self.wait()
 