require 'fast_uuid'
require 'securerandom'

puts FastUUID::VERSION
uuid = SecureRandom.uuid.upcase
puts uuid
puts FastUUID.uuid?(uuid)
puts FastUUID.uuid?('xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx')
