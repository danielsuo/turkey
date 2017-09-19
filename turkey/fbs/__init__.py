import flatbuffers

from Turkey.Message import Message, MessageStart, MessageAddType, \
        MessageAddData, MessageEnd


def encodeMessage(messageType, data):
    builder = flatbuffers.Builder(1024)
    MessageStart(builder)
    MessageAddType(builder, messageType)
    MessageAddData(builder, data)
    reply = MessageEnd(builder)
    builder.Finish(reply)

    return builder.Output()


def decodeMessage(buf):
    buf = bytearray(buf)
    return Message.GetRootAsMessage(buf, 0)
