import socket 
import time
from abc import ABC, abstractmethod

HOST = "XXXXX"  # Standard loopback interface address (localhost)
PORT = XXXX  # Port to listen on (non-privileged ports are > 1023)
    
class TCP(ABC):
    @abstractmethod
    def waitForConnection(self):
        pass
    @abstractmethod
    def readBytes(self, numBytes : int):
        pass
    @abstractmethod
    def readHeader(self) -> str:
        pass
    @abstractmethod
    def readBody(self,numBytes : int) -> str:
        pass
    @abstractmethod
    def sendBytes(self, bytes : bytes):
        pass
    @abstractmethod
    def closeConnection(self):
        pass

class TCPHandler(TCP):
  
    def __init__(self) -> None:
        self._s  = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
        self._s.bind((HOST, PORT))
        self._s.listen()
    
    def waitForConnection(self):
        conn, addr= self._s.accept()
        self._conn = conn
        return conn
    
    def readBytes(self, numBytes : int):
        return self._conn.recv(numBytes)
    
    def readHeader(self) -> str:
        return self._conn.recv(6).decode("utf-8")
    
    def readBody(self,numBytes : int) -> str:
        return self._conn.recv(numBytes).decode("utf-8")
    
    def sendBytes(self, bytes : bytes):
        self._conn.sendall(bytes)

    def closeConnection(self):
        self._conn.shutdown(socket.SHUT_RDWR)
        self._conn.close()

class MockTCPHandler(TCP):

    def __init__(self) -> None:
        pass
    
    def waitForConnection(self):
        time.sleep(5)
        return None
    
    def readBytes(self, numBytes : int):
        return "self._conn.recv(numBytes)"
    
    def readHeader(self) -> str:
        return "010010"
    
    def readBody(self,numBytes : int) -> str:
        return "23.6;10.82"
    
    def sendBytes(self, bytes : bytes):
        pass
    def closeConnection(self):
        pass

class TCPHandlerFactory:
    @staticmethod
    def createConnectionHandler(real : int)  -> TCP:
        if real == 0:
            return MockTCPHandler()
        else:
            return TCPHandler()