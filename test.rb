require 'fast_uuid'
require 'securerandom'

puts FastUUID::VERSION
uuid = SecureRandom.uuid.upcase
puts uuid

# valid uuid
puts FastUUID.uuid?(uuid)

# invalid uuid
puts FastUUID.uuid?('xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx')
