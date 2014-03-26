; Auto-generated. Do not edit!


(in-package swistrack-msg)


;//! \htmlinclude ParticleTrack.msg.html

(defclass <ParticleTrack> (ros-message)
  ((header
    :reader header-val
    :initarg :header
    :type roslib-msg:<Header>
    :initform (make-instance 'roslib-msg:<Header>))
   (ID
    :reader ID-val
    :initarg :ID
    :type integer
    :initform 0)
   (IDCovariance
    :reader IDCovariance-val
    :initarg :IDCovariance
    :type float
    :initform 0.0)
   (xCenter
    :reader xCenter-val
    :initarg :xCenter
    :type float
    :initform 0.0)
   (yCenter
    :reader yCenter-val
    :initarg :yCenter
    :type float
    :initform 0.0)
   (orientation
    :reader orientation-val
    :initarg :orientation
    :type float
    :initform 0.0)
   (area
    :reader area-val
    :initarg :area
    :type float
    :initform 0.0)
   (compactness
    :reader compactness-val
    :initarg :compactness
    :type float
    :initform 0.0)
   (xWorldCenter
    :reader xWorldCenter-val
    :initarg :xWorldCenter
    :type float
    :initform 0.0)
   (yWorldCenter
    :reader yWorldCenter-val
    :initarg :yWorldCenter
    :type float
    :initform 0.0)
   (frameNumber
    :reader frameNumber-val
    :initarg :frameNumber
    :type integer
    :initform 0)
   (timeStamp
    :reader timeStamp-val
    :initarg :timeStamp
    :type integer
    :initform 0))
)
(defmethod serialize ((msg <ParticleTrack>) ostream)
  "Serializes a message object of type '<ParticleTrack>"
  (serialize (slot-value msg 'header) ostream)
    (write-byte (ldb (byte 8 0) (slot-value msg 'ID)) ostream)
  (write-byte (ldb (byte 8 8) (slot-value msg 'ID)) ostream)
  (write-byte (ldb (byte 8 16) (slot-value msg 'ID)) ostream)
  (write-byte (ldb (byte 8 24) (slot-value msg 'ID)) ostream)
  (let ((bits (roslisp-utils:encode-single-float-bits (slot-value msg 'IDCovariance))))
    (write-byte (ldb (byte 8 0) bits) ostream)
    (write-byte (ldb (byte 8 8) bits) ostream)
    (write-byte (ldb (byte 8 16) bits) ostream)
    (write-byte (ldb (byte 8 24) bits) ostream))
  (let ((bits (roslisp-utils:encode-single-float-bits (slot-value msg 'xCenter))))
    (write-byte (ldb (byte 8 0) bits) ostream)
    (write-byte (ldb (byte 8 8) bits) ostream)
    (write-byte (ldb (byte 8 16) bits) ostream)
    (write-byte (ldb (byte 8 24) bits) ostream))
  (let ((bits (roslisp-utils:encode-single-float-bits (slot-value msg 'yCenter))))
    (write-byte (ldb (byte 8 0) bits) ostream)
    (write-byte (ldb (byte 8 8) bits) ostream)
    (write-byte (ldb (byte 8 16) bits) ostream)
    (write-byte (ldb (byte 8 24) bits) ostream))
  (let ((bits (roslisp-utils:encode-single-float-bits (slot-value msg 'orientation))))
    (write-byte (ldb (byte 8 0) bits) ostream)
    (write-byte (ldb (byte 8 8) bits) ostream)
    (write-byte (ldb (byte 8 16) bits) ostream)
    (write-byte (ldb (byte 8 24) bits) ostream))
  (let ((bits (roslisp-utils:encode-double-float-bits (slot-value msg 'area))))
    (write-byte (ldb (byte 8 0) bits) ostream)
    (write-byte (ldb (byte 8 8) bits) ostream)
    (write-byte (ldb (byte 8 16) bits) ostream)
    (write-byte (ldb (byte 8 24) bits) ostream)
    (write-byte (ldb (byte 8 32) bits) ostream)
    (write-byte (ldb (byte 8 40) bits) ostream)
    (write-byte (ldb (byte 8 48) bits) ostream)
    (write-byte (ldb (byte 8 56) bits) ostream))
  (let ((bits (roslisp-utils:encode-double-float-bits (slot-value msg 'compactness))))
    (write-byte (ldb (byte 8 0) bits) ostream)
    (write-byte (ldb (byte 8 8) bits) ostream)
    (write-byte (ldb (byte 8 16) bits) ostream)
    (write-byte (ldb (byte 8 24) bits) ostream)
    (write-byte (ldb (byte 8 32) bits) ostream)
    (write-byte (ldb (byte 8 40) bits) ostream)
    (write-byte (ldb (byte 8 48) bits) ostream)
    (write-byte (ldb (byte 8 56) bits) ostream))
  (let ((bits (roslisp-utils:encode-single-float-bits (slot-value msg 'xWorldCenter))))
    (write-byte (ldb (byte 8 0) bits) ostream)
    (write-byte (ldb (byte 8 8) bits) ostream)
    (write-byte (ldb (byte 8 16) bits) ostream)
    (write-byte (ldb (byte 8 24) bits) ostream))
  (let ((bits (roslisp-utils:encode-single-float-bits (slot-value msg 'yWorldCenter))))
    (write-byte (ldb (byte 8 0) bits) ostream)
    (write-byte (ldb (byte 8 8) bits) ostream)
    (write-byte (ldb (byte 8 16) bits) ostream)
    (write-byte (ldb (byte 8 24) bits) ostream))
    (write-byte (ldb (byte 8 0) (slot-value msg 'frameNumber)) ostream)
  (write-byte (ldb (byte 8 8) (slot-value msg 'frameNumber)) ostream)
  (write-byte (ldb (byte 8 16) (slot-value msg 'frameNumber)) ostream)
  (write-byte (ldb (byte 8 24) (slot-value msg 'frameNumber)) ostream)
    (write-byte (ldb (byte 8 0) (slot-value msg 'timeStamp)) ostream)
  (write-byte (ldb (byte 8 8) (slot-value msg 'timeStamp)) ostream)
  (write-byte (ldb (byte 8 16) (slot-value msg 'timeStamp)) ostream)
  (write-byte (ldb (byte 8 24) (slot-value msg 'timeStamp)) ostream)
)
(defmethod deserialize ((msg <ParticleTrack>) istream)
  "Deserializes a message object of type '<ParticleTrack>"
  (deserialize (slot-value msg 'header) istream)
  (setf (ldb (byte 8 0) (slot-value msg 'ID)) (read-byte istream))
  (setf (ldb (byte 8 8) (slot-value msg 'ID)) (read-byte istream))
  (setf (ldb (byte 8 16) (slot-value msg 'ID)) (read-byte istream))
  (setf (ldb (byte 8 24) (slot-value msg 'ID)) (read-byte istream))
  (let ((bits 0))
    (setf (ldb (byte 8 0) bits) (read-byte istream))
    (setf (ldb (byte 8 8) bits) (read-byte istream))
    (setf (ldb (byte 8 16) bits) (read-byte istream))
    (setf (ldb (byte 8 24) bits) (read-byte istream))
    (setf (slot-value msg 'IDCovariance) (roslisp-utils:decode-single-float-bits bits)))
  (let ((bits 0))
    (setf (ldb (byte 8 0) bits) (read-byte istream))
    (setf (ldb (byte 8 8) bits) (read-byte istream))
    (setf (ldb (byte 8 16) bits) (read-byte istream))
    (setf (ldb (byte 8 24) bits) (read-byte istream))
    (setf (slot-value msg 'xCenter) (roslisp-utils:decode-single-float-bits bits)))
  (let ((bits 0))
    (setf (ldb (byte 8 0) bits) (read-byte istream))
    (setf (ldb (byte 8 8) bits) (read-byte istream))
    (setf (ldb (byte 8 16) bits) (read-byte istream))
    (setf (ldb (byte 8 24) bits) (read-byte istream))
    (setf (slot-value msg 'yCenter) (roslisp-utils:decode-single-float-bits bits)))
  (let ((bits 0))
    (setf (ldb (byte 8 0) bits) (read-byte istream))
    (setf (ldb (byte 8 8) bits) (read-byte istream))
    (setf (ldb (byte 8 16) bits) (read-byte istream))
    (setf (ldb (byte 8 24) bits) (read-byte istream))
    (setf (slot-value msg 'orientation) (roslisp-utils:decode-single-float-bits bits)))
  (let ((bits 0))
    (setf (ldb (byte 8 0) bits) (read-byte istream))
    (setf (ldb (byte 8 8) bits) (read-byte istream))
    (setf (ldb (byte 8 16) bits) (read-byte istream))
    (setf (ldb (byte 8 24) bits) (read-byte istream))
    (setf (ldb (byte 8 32) bits) (read-byte istream))
    (setf (ldb (byte 8 40) bits) (read-byte istream))
    (setf (ldb (byte 8 48) bits) (read-byte istream))
    (setf (ldb (byte 8 56) bits) (read-byte istream))
    (setf (slot-value msg 'area) (roslisp-utils:decode-double-float-bits bits)))
  (let ((bits 0))
    (setf (ldb (byte 8 0) bits) (read-byte istream))
    (setf (ldb (byte 8 8) bits) (read-byte istream))
    (setf (ldb (byte 8 16) bits) (read-byte istream))
    (setf (ldb (byte 8 24) bits) (read-byte istream))
    (setf (ldb (byte 8 32) bits) (read-byte istream))
    (setf (ldb (byte 8 40) bits) (read-byte istream))
    (setf (ldb (byte 8 48) bits) (read-byte istream))
    (setf (ldb (byte 8 56) bits) (read-byte istream))
    (setf (slot-value msg 'compactness) (roslisp-utils:decode-double-float-bits bits)))
  (let ((bits 0))
    (setf (ldb (byte 8 0) bits) (read-byte istream))
    (setf (ldb (byte 8 8) bits) (read-byte istream))
    (setf (ldb (byte 8 16) bits) (read-byte istream))
    (setf (ldb (byte 8 24) bits) (read-byte istream))
    (setf (slot-value msg 'xWorldCenter) (roslisp-utils:decode-single-float-bits bits)))
  (let ((bits 0))
    (setf (ldb (byte 8 0) bits) (read-byte istream))
    (setf (ldb (byte 8 8) bits) (read-byte istream))
    (setf (ldb (byte 8 16) bits) (read-byte istream))
    (setf (ldb (byte 8 24) bits) (read-byte istream))
    (setf (slot-value msg 'yWorldCenter) (roslisp-utils:decode-single-float-bits bits)))
  (setf (ldb (byte 8 0) (slot-value msg 'frameNumber)) (read-byte istream))
  (setf (ldb (byte 8 8) (slot-value msg 'frameNumber)) (read-byte istream))
  (setf (ldb (byte 8 16) (slot-value msg 'frameNumber)) (read-byte istream))
  (setf (ldb (byte 8 24) (slot-value msg 'frameNumber)) (read-byte istream))
  (setf (ldb (byte 8 0) (slot-value msg 'timeStamp)) (read-byte istream))
  (setf (ldb (byte 8 8) (slot-value msg 'timeStamp)) (read-byte istream))
  (setf (ldb (byte 8 16) (slot-value msg 'timeStamp)) (read-byte istream))
  (setf (ldb (byte 8 24) (slot-value msg 'timeStamp)) (read-byte istream))
  msg
)
(defmethod ros-datatype ((msg (eql '<ParticleTrack>)))
  "Returns string type for a message object of type '<ParticleTrack>"
  "swistrack/ParticleTrack")
(defmethod md5sum ((type (eql '<ParticleTrack>)))
  "Returns md5sum for a message object of type '<ParticleTrack>"
  "75efc74a6dface65452db4fa5a337ca5")
(defmethod message-definition ((type (eql '<ParticleTrack>)))
  "Returns full string definition for message of type '<ParticleTrack>"
  (format nil "Header header~%int32 ID~%float32 IDCovariance~%float32 xCenter~%float32 yCenter~%float32 orientation~%float64 area~%float64 compactness~%float32 xWorldCenter~%float32 yWorldCenter~%int32 frameNumber~%int32 timeStamp~%~%================================================================================~%MSG: roslib/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.secs: seconds (stamp_secs) since epoch~%# * stamp.nsecs: nanoseconds since stamp_secs~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%# 0: no frame~%# 1: global frame~%string frame_id~%~%~%"))
(defmethod serialization-length ((msg <ParticleTrack>))
  (+ 0
     (serialization-length (slot-value msg 'header))
     4
     4
     4
     4
     4
     8
     8
     4
     4
     4
     4
))
(defmethod ros-message-to-list ((msg <ParticleTrack>))
  "Converts a ROS message object to a list"
  (list '<ParticleTrack>
    (cons ':header (header-val msg))
    (cons ':ID (ID-val msg))
    (cons ':IDCovariance (IDCovariance-val msg))
    (cons ':xCenter (xCenter-val msg))
    (cons ':yCenter (yCenter-val msg))
    (cons ':orientation (orientation-val msg))
    (cons ':area (area-val msg))
    (cons ':compactness (compactness-val msg))
    (cons ':xWorldCenter (xWorldCenter-val msg))
    (cons ':yWorldCenter (yWorldCenter-val msg))
    (cons ':frameNumber (frameNumber-val msg))
    (cons ':timeStamp (timeStamp-val msg))
))
