
(in-package :asdf)

(defsystem "swistrack-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :roslib-msg
)
  :components ((:file "_package")
    (:file "ParticleTrack" :depends-on ("_package"))
    (:file "_package_ParticleTrack" :depends-on ("_package"))
    ))
