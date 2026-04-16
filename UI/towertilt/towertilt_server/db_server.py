# !/usr/bin/python3
# coding: utf-8

import jaydebeapi
from collections import namedtuple

InclinationRecord = namedtuple('InclinationRecord', ['did', 'ts', 'x', 'y', 'z'])

class DbServer(object):
	"""docstring for DbServer"""
	def __init__(self):
		super(DbServer, self).__init__()
		driver, url, username, passwd, jar = ('org.h2.Driver', 'jdbc:h2:./db', 'sa', 'sa', './h2/bin/h2-2.2.222.jar')
		self.conn = jaydebeapi.connect(driver, url, [username, passwd], jar)
		self.cursor = self.conn.cursor()
		self.create_inclination_record_table()
		print('[H2] Db init success.')
		self.ent2obj = lambda ent: InclinationRecord(ent[0], ent[1], ent[2], ent[3], ent[4])

	def create_inclination_record_table(self):
		# self.cursor.execute("DROP TABLE IF EXISTS INCLINATION_RECORD")
		self.cursor.execute("""
			CREATE TABLE IF NOT EXISTS INCLINATION_RECORD (
				did int not null,
				ts int not null,
				x float default 0,
				y float default 0,
				z float default 0,
				primary key(did, ts)
			)
		""")
		print('[H2] Create table if not exists `INCLINATION_RECORD` success.')

	def query_inclination_record(self, did, ts_s, ts_e):
		self.cursor.execute('SELECT * FROM INCLINATION_RECORD WHERE DID = ? AND TS >= ? AND TS <= ?', (did, ts_s, ts_e))
		results = self.cursor.fetchall()
		return list(map(lambda x: self.ent2obj(x), results))

	def insert_inclination_record(self, ent: InclinationRecord):
		self.cursor.execute("INSERT INTO INCLINATION_RECORD VALUES(?, ?, ?, ?, ?)", (ent.did, ent.ts, ent.x, ent.y, ent.z))

	def close(self):
		self.conn.close()

if __name__ == '__main__':
	inst = DbServer()
	# inst.create_inclination_record_table()
