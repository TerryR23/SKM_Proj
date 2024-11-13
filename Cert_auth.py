import random as rand
def getBytes(x:int):
  return rand.randbyte(x)



'''This class is the certificate and is setting it up. will only be created by the certifcate authority and should be unique to a device or a pair of devices. (will later be modified to include support for a device that will create a certificate for new devices)'''
class Certificate():
  def __init__(self):
    self.ID = self.ID_create()
    self.KDK = bytearray
    self.MK = bytearray
    
'''The Function below will take the Two devices that are trying to communicate ad create a KDK value between them. This function is being moved to the Devices to support Custom ordering of inputs for HASH function.'''
  def KDK_create(self, ID_A:bytes, ID_B:bytes):
    if (self.ID == None):
      self.ID = self.ID_create()
      

  def ID_create(self):
    '''This fucction will be used to create the ID that a device may need.'''
    if (self.Host == True):
      ID = b'Serv' + getBytes(28)
    else:
      ID = b'Dev' + getBytes(29)
    return ID


'''This class will be used to simulate each device on the network. When a device is added, it will be given a Certificate and should have a name (For now the name will be given but may eventually take on the device specs)'''
class Device():
  def __init__(self, certificate:Certificate, Name):
    self.certificate = certificate
    self.PubKey = b''
    self.MPubKey= b''
    self.Status = [1,0] '''[client,Host] tells you whether the device is configured to be a client or a host and if the certificate has already been duplicated'''
    self.Host = False
    self.ID = certificate.ID_create()
    self.Name = Name

  def KDK_create(self, ID_Cert, ID_B):
    '''self will be the devices person ID value; ID_cert will contain the ID value of the certificate; ID_B will be the ID value of the device it wishes to comminicate with'''
    self.Certificate.KDK = SHA256(self.ID, self.certificate.ID, ID_B)
    return  
  